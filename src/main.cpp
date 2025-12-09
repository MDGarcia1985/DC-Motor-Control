/**
 * @file main.cpp
 * @brief DC Motor Control with Dynamic Distance Mapping + Timed Maneuver Logic
 * @version 3.0.0
 * 
 * Behavior Summary:
 *  - Normal mode:
 *        Distance dynamically maps to speed 100 → 0% using 2% quantization.
 *  - When distance >= MAX_DIST_CM:
 *        Enter timed maneuver:
 *           STOP (500ms) → REVERSE (200ms) → SLOW-FWD (until next valid reading)
 *  - All transitions are NON-BLOCKING (millis-based), no delay()
 * 
 * System Components:
 *  - HC-SR04 ultrasonic
 *  - L293D motor driver
 *  - StatusLED gradient + error + blinking transitions
 *  - SSD1306 OLED status display
 */

#include <Arduino.h>
#include "motor.h"
#include "ultraSonic.h"
#include "led.h"
#include "display.h"

// -----------------------------------------------------------------------------
// Control parameters
// -----------------------------------------------------------------------------

static const unsigned long UPDATE_INTERVAL_MS = 10;  // main loop pacing
static const unsigned long STOP_TIME_MS       = 500; // stop before reversing
static const unsigned long REVERSE_TIME_MS    = 200; // reverse duration

static const int MIN_DIST_CM = 5;    // distance where commanded = 0%
static const int MAX_DIST_CM = 60;   // distance where commanded = 100%

// -----------------------------------------------------------------------------
// Hardware objects
// -----------------------------------------------------------------------------

Motor      motor;
UltraSonic usonic;
StatusLED  statusLed;
Display    display;

// -----------------------------------------------------------------------------
// Motor maneuver state machine
// -----------------------------------------------------------------------------

enum MotorMode { NORMAL, STOPPING, REVERSING, SLOW_FORWARD };
MotorMode motorMode = NORMAL;

unsigned long modeStartMs = 0;
unsigned long lastUpdateMs = 0;

int lastSpeedPct = 100;
bool errorState = false;

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup()
{
    motor.begin();
    usonic.begin();
    statusLed.begin();
    display.begin();

    lastSpeedPct = 100;
    motor.setSpeed(lastSpeedPct);
    statusLed.update(lastSpeedPct);
    display.update(0, lastSpeedPct, false);

    lastUpdateMs = millis();
}

// -----------------------------------------------------------------------------
// LOOP
// -----------------------------------------------------------------------------

void loop()
{
    unsigned long now = millis();

    if (now - lastUpdateMs < UPDATE_INTERVAL_MS)
        return;
    lastUpdateMs = now;

    // ================================================================
    // Read ultrasonic distance
    // ================================================================
    int distance = usonic.readCM();
    int speedPct = lastSpeedPct;

    // ================================================================
    //  MOTOR MANEUVER STATE MACHINE
    // ================================================================
    if (distance >= MAX_DIST_CM)
    {
        switch (motorMode)
        {
            // ---------------------------------------------------------
            // entering stop phase
            // ---------------------------------------------------------
            case NORMAL:
                speedPct = 0;
                motorMode = STOPPING;
                modeStartMs = now;
                break;

            // ---------------------------------------------------------
            // stopping before reverse
            // ---------------------------------------------------------
            case STOPPING:
                speedPct = 0;
                if (now - modeStartMs >= STOP_TIME_MS)
                {
                    motorMode = REVERSING;
                    modeStartMs = now;
                }
                break;

            // ---------------------------------------------------------
            // reverse for fixed time
            // ---------------------------------------------------------
            case REVERSING:
                speedPct = -20;   // controlled reverse
                if (now - modeStartMs >= REVERSE_TIME_MS)
                {
                    motorMode = SLOW_FORWARD;
                    modeStartMs = now;
                }
                break;

            // ---------------------------------------------------------
            // creep forward until next meaningful reading
            // ---------------------------------------------------------
            case SLOW_FORWARD:
                speedPct = 20;

                // If obstruction condition returns to NORMAL range,
                // abort maneuver immediately:
                if (distance < MAX_DIST_CM)
                {
                    motorMode = NORMAL;
                }
                break;
        }
    }
    else
    {
        // ============================================================
        // RETURN TO NORMAL MODE (dynamic scaling)
        // ============================================================

        motorMode = NORMAL;
        modeStartMs = now;

        // Dynamic mapping 0–100%
        speedPct = map(distance, MIN_DIST_CM, MAX_DIST_CM, 0, 100);

        // Quantize for smooth motor + smooth LED
        speedPct = (speedPct / 2) * 2;
        speedPct = constrain(speedPct, 0, 100);

        // If too close, clamp to 0
        if (distance <= MIN_DIST_CM)
            speedPct = 0;

        // 0 reading = free path
        if (distance == 0)
            speedPct = 100;
    }

    lastSpeedPct = speedPct;
    errorState   = false;   // reserved for later diagnostics

    // ================================================================
    // APPLY OUTPUTS (non-blocking)
    // ================================================================
    motor.setSpeed(speedPct);
    statusLed.setError(errorState);
    statusLed.update(speedPct);
    display.update(distance, speedPct, errorState);
}
