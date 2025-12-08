/**
 * @file motor.cpp
 * @brief Implementation of DC motor control
 * 
 * @author Michael Garcia, M&E Design
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#include "motor.h"

void Motor::begin()
{
    // Configure motor control pins as outputs
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
}

void Motor::setSpeed(int percent)
{
    // Constrain input to valid range
    percent = constrain(percent, -100, 100);
    
    // Convert percentage to PWM value (0-255)
    int pwmValue = map(abs(percent), 0, 100, 0, 255);

    if (percent >= 0)
    {
        // Forward: IN1 = PWM, IN2 = LOW
        analogWrite(IN1, pwmValue);
        analogWrite(IN2, 0);
    }
    else
    {
        // Reverse: IN1 = LOW, IN2 = PWM
        analogWrite(IN1, 0);
        analogWrite(IN2, pwmValue);
    }
}
