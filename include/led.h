/**
 * @file led.h
 * @brief RGB LED status indicator with smooth color transitions
 * 
 * Displays motor speed via color gradient:
 * - Red (0%) -> Yellow (50%) -> Green (100%)
 * - Smooth transitions with blinking effect on 50->0 transition
 * 
 * @author Michael Garcia, M&E Design
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#pragma once
#include <Arduino.h>

class StatusLED
{
public:
    /**
     * @brief Initialize RGB LED pins
     */
    void begin();
    
    /**
     * @brief Update LED color based on motor speed
     * @param commandedPercent Motor speed 0-100%
     */
    void update(int commandedPercent);
    
    /**
     * @brief Set error mode (blue LED blink)
     * @param active True to enable error mode, false to disable
     */
    void setError(bool active);

private:
    static const uint8_t PIN_R = 3;   // Red LED on D3 (PWM)
    static const uint8_t PIN_G = 11;  // Green LED on D11 (PWM)
    static const uint8_t PIN_B = 2;   // Blue LED on D2 (error indicator)
    static const unsigned long STEP_INTERVAL_MS = 20;  // Color transition step delay
    static const unsigned long BLINK_HALF_PERIOD_MS = 125;  // Blink timing
    static const unsigned long ERROR_BLINK_MS = 500;  // Error blink period
    static const uint8_t MAX_BRIGHTNESS = 255;
    static const uint8_t DIM_BRIGHTNESS = 60;
    static const int COLOR_TRANSITION_THRESHOLD = 50;  // Midpoint for color gradient

    int currentPercent = 0;    // Current LED state (0-100)
    int targetPercent = 0;     // Target LED state (0-100)
    int lastCommand = 0;       // Last commanded speed
    bool blinkingToZero = false;  // Special blink mode for 50->0
    bool lastBlinkState = false;  // Blink toggle state
    bool errorActive = false;      // Error mode active
    bool errorBlinkOn = false;     // Error blink state
    unsigned long lastStepMs = 0;       // Timer for color transitions
    unsigned long lastBlinkMs = 0;      // Timer for blinking
    unsigned long lastErrorBlinkMs = 0; // Timer for error blink

    /**
     * @brief Set RGB LED color values
     * @param r Red intensity (0-255)
     * @param g Green intensity (0-255)
     * @param b Blue intensity (0-255)
     */
    void setRGB(uint8_t r, uint8_t g, uint8_t b);
};
