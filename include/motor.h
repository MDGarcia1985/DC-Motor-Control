/**
 * @file motor.h
 * @brief DC Motor control using L293D motor driver
 * @version 2.1.0
 * 
 * Controls motor direction and speed via PWM on IN1/IN2.
 * Supports bidirectional control:
 *  - percent > 0 : forward
 *  - percent < 0 : reverse
 *  - percent = 0 : stop (coast)
 * 
 * EN pin is tied high with 10kΩ resistor (always enabled).
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
     * @brief Initialize motor control pins.
     * Configures IN1 and IN2 as OUTPUT and stops the motor.
     */
    void begin();
    
    /**
     * @brief Set motor speed and direction.
     * 
     * @param percent Speed from -100 (full reverse) to 100 (full forward).
     *                Values within a small deadband around 0 are treated as 0.
     */
    void setSpeed(int percent);

    /**
     * @brief Immediately stop the motor (coast).
     * Equivalent to setSpeed(0).
     */
    void stop();

    /**
     * @brief Actively brake the motor by shorting both outputs to GND.
     * 
     * Note: With EN held high and both inputs LOW, both outputs go low,
     * providing a braking effect rather than free coasting.
     */
    void brake();

private:
    static const uint8_t IN1 = 9;   ///< L293D Input 1 (PWM capable)
    static const uint8_t IN2 = 10;  ///< L293D Input 2 (PWM capable)

    /// @brief Minimum absolute percent considered "moving".
    ///        Values with |percent| < DEAD_BAND_PERCENT are treated as 0.
    static const uint8_t DEAD_BAND_PERCENT = 3;

    int lastCommandPercent = 0;     ///< Last commanded speed [-100..100]

    void applyOutputs(int pwmValue, bool forward);
};
