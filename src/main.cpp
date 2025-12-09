/**
 * @file main.cpp
 * @brief DC Motor Control with Ultrasonic Distance Sensing
 * @version 2.0.0
 * 
 * Controls motor speed based on ultrasonic distance measurements:
 * - Distance < 10cm: Motor reverses at 50% (-50%)
 * - Distance 10-30cm: Motor forward at medium speed (50%)
 * - Distance > 30cm: Motor forward at full speed (100%)
 * - No object detected (timeout): Motor forward at full speed (100%)
 * 
 * RGB LED provides visual feedback:
 * - Red (0%): Reverse operation
 * - Yellow (50%): Medium speed forward (blinking during 50%->0% transition)
 * - Green (100%): Full speed forward
 * 
 * OLED Display shows:
 * - Distance measurement (cm) or "No Obstruction"
 * - PWM output percentage with direction (FWD/REV)
 * - Visual status bar for motor speed
 * 
 * @author Michael Garcia, M&E Design
 * @contact michael@mandedesign.studio
 * @website www.mandedesign.studio
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#include <Arduino.h>
#include "motor.h"
#include "ultraSonic.h"
#include "led.h"
#include "display.h"

// Hardware objects
Motor motor;
UltraSonic usonic;
StatusLED statusLed;
Display display;

// State variables
unsigned long lastUpdate = 0;
int lastSpeedPct = 100;  // Start at 100% (no obstruction)
bool error = false;

void setup()
{
    // Initialize all hardware components
    motor.begin();
    usonic.begin();
    statusLed.begin();
    display.begin();
    
    // Set initial state to 100% (no obstruction)
    motor.setSpeed(100);
    statusLed.update(100);
    display.update(0, 100, false);
}

void loop()
{
    unsigned long now = millis();
    
    // Update at 100Hz (every 10ms)
    if (now - lastUpdate >= 10)
    {
        lastUpdate = now;
        
        // Read distance from ultrasonic sensor
        int distance = usonic.readCM();
        int speedPct;
        
        // Update speed based on distance thresholds
        if (distance == 0)
        {
            // No object detected (timeout) - full speed forward
            speedPct = 100;
        }
        else if (distance < 10)
        {
            // Reverse zone: Red LED
            speedPct = -50;
        }
        else if (distance < 30)
        {
            // Medium speed zone: Blinking Yellow
            speedPct = 50;
        }
        else
        {
            // Full speed zone: Green LED (>30cm)
            speedPct = 100;
        }
        
        lastSpeedPct = speedPct;
        error = false;
        
        // Apply speed to motor and update indicators
        motor.setSpeed(speedPct);
        statusLed.update(speedPct);
        display.update(distance, speedPct, error);
    }
}
