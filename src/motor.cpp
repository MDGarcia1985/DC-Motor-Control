/**
 * @file motor.cpp
 * @brief Implementation of DC motor control
 * @version 2.1.0
 * 
 * Uses L293D with EN tied high. Direction and speed are controlled
 * by PWM on IN1 and IN2 as follows:
 * 
 *  - Forward:  IN1 = PWM, IN2 = LOW
 *  - Reverse:  IN1 = LOW, IN2 = PWM
 *  - Stop:     IN1 = LOW, IN2 = LOW  (coast)
 *  - Brake:    IN1 = LOW, IN2 = LOW  (with EN high, outputs low → brake to GND)
 * 
 * Deadband is applied around 0 to avoid tiny duty cycles causing
 * audible whine or ineffective motion.
 * 
 * @author Michael Garcia, M&E Design
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#include "motor.h"

void Motor::begin()
{
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    stop();
}

void Motor::applyOutputs(int pwmValue, bool forward)
{
    // pwmValue is 0..255
    if (pwmValue <= 0)
    {
        // Coast / stop: both inputs LOW
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        return;
    }

    if (forward)
    {
        // Forward: IN1 PWM, IN2 LOW
        analogWrite(IN1, pwmValue);
        digitalWrite(IN2, LOW);
    }
    else
    {
        // Reverse: IN1 LOW, IN2 PWM
        digitalWrite(IN1, LOW);
        analogWrite(IN2, pwmValue);
    }
}

void Motor::setSpeed(int percent)
{
    // Constrain input to valid range
    percent = constrain(percent, -100, 100);

    // Apply deadband around zero
    if (abs(percent) < DEAD_BAND_PERCENT)
    {
        percent = 0;
    }

    // If the command hasn't changed, avoid redundant writes
    if (percent == lastCommandPercent)
    {
        return;
    }
    lastCommandPercent = percent;

    if (percent == 0)
    {
        // Stop/coast
        applyOutputs(0, true);
        return;
    }

    bool forward = (percent > 0);
    int magnitude = abs(percent);

    // Map 1..100% to 1..255 duty; ensure non-zero when we say "move"
    int pwmValue = map(magnitude, 1, 100, 1, 255);
    applyOutputs(pwmValue, forward);
}

void Motor::stop()
{
    lastCommandPercent = 0;
    applyOutputs(0, true);
}

void Motor::brake()
{
    // With EN tied high, both inputs LOW make both outputs LOW -> braking.
    lastCommandPercent = 0;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
}
