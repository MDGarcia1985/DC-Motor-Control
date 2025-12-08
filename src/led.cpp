/**
 * @file led.cpp
 * @brief Implementation of RGB LED status indicator
 * @version 1.0.0
 * 
 * @author Michael Garcia, M&E Design
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#include "led.h"

void StatusLED::begin()
{
    // Configure RGB pins as outputs
    pinMode(PIN_R, OUTPUT);
    pinMode(PIN_G, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    
    // Initialize LED to off state
    setRGB(0, 0, 0);
}

void StatusLED::setRGB(uint8_t r, uint8_t g, uint8_t b)
{
    // Write PWM values to RGB pins
    analogWrite(PIN_R, r);
    analogWrite(PIN_G, g);
    analogWrite(PIN_B, b);
}

void StatusLED::setError(bool active)
{
    if (active && !errorActive)
    {
        // Entering error mode: reset timing
        lastErrorBlinkMs = millis();
        errorBlinkOn = false;
    }
    errorActive = active;
}

void StatusLED::update(int commandedPercent)
{
    unsigned long now = millis();

    // Error mode: blue blink @ 500ms
    if (errorActive)
    {
        if (now - lastErrorBlinkMs >= ERROR_BLINK_MS)
        {
            lastErrorBlinkMs = now;
            errorBlinkOn = !errorBlinkOn;
        }

        // Blue on/off, R & G off
        uint8_t b = errorBlinkOn ? MAX_BRIGHTNESS : 0;
        setRGB(0, 0, b);
        return;
    }

    // Constrain input to valid range
    commandedPercent = constrain(commandedPercent, 0, 100);

    // Detect speed change and transition type
    if (commandedPercent != targetPercent)
    {
        // Enable special blinking effect for 50% -> 0% transition
        if (lastCommand == 50 && commandedPercent == 0)
        {
            blinkingToZero = true;
        }
        else
        {
            blinkingToZero = false;
        }

        targetPercent = commandedPercent;
        lastCommand = commandedPercent;
    }

    // Smooth transition: increment/decrement currentPercent toward target
    if (now - lastStepMs >= STEP_INTERVAL_MS)
    {
        lastStepMs = now;

        if (currentPercent < targetPercent) currentPercent++;
        else if (currentPercent > targetPercent) currentPercent--;
    }

    // Map currentPercent (0-100) to RGB color gradient
    // 0% = Red (255,0,0), 50% = Yellow (255,255,0), 100% = Green (0,255,0)
    uint8_t r, g, b;
    b = 0;  // Blue channel unused

    if (currentPercent <= COLOR_TRANSITION_THRESHOLD)
    {
        // Red -> Yellow: red stays max, green ramps up
        float t = currentPercent / (float)COLOR_TRANSITION_THRESHOLD;
        r = MAX_BRIGHTNESS;
        g = (uint8_t)(MAX_BRIGHTNESS * t);
    }
    else
    {
        // Yellow -> Green: green stays max, red ramps down
        float t = (currentPercent - COLOR_TRANSITION_THRESHOLD) / (float)COLOR_TRANSITION_THRESHOLD;
        r = (uint8_t)(MAX_BRIGHTNESS * (1.0f - t));
        g = MAX_BRIGHTNESS;
    }

    // Apply blinking effect during 50% -> 0% transition
    if (blinkingToZero && currentPercent > 0)
    {
        // Toggle blink state at regular intervals
        if (now - lastBlinkMs >= BLINK_HALF_PERIOD_MS)
        {
            lastBlinkMs = now;
            lastBlinkState = !lastBlinkState;
        }
        
        // Scale brightness between max and dim
        uint8_t scale = lastBlinkState ? MAX_BRIGHTNESS : DIM_BRIGHTNESS;
        r = (uint8_t)((uint16_t)r * scale / MAX_BRIGHTNESS);
        g = (uint8_t)((uint16_t)g * scale / MAX_BRIGHTNESS);
    }

    // Update LED with calculated color
    setRGB(r, g, b);
}
