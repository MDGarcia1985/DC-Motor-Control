/**
 * @file motor.h
 * @brief DC Motor control using L293D motor driver
 * @version 1.0.1
 * 
 * Controls motor direction and speed via PWM on analog pins A0 and A1.
 * Supports bidirectional control: positive values for forward, negative for reverse.
 * 
 * @author Michael Garcia, M&E Design
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#pragma once
#include <Arduino.h>

class Motor
{
public:
    /**
     * @brief Initialize motor control pins
     * Sets IN1 and IN2 as OUTPUT pins
     */
    void begin();
    
    /**
     * @brief Set motor speed and direction
     * @param percent Speed from -100 (full reverse) to 100 (full forward), 0 = stop
     */
    void setSpeed(int percent);

private:
    static const uint8_t IN1 = 9;   // L293D Input 1 (PWM)
    static const uint8_t IN2 = 10;  // L293D Input 2 (PWM)
    // EN pin tied high with 10kΩ resistor (always enabled)
};
