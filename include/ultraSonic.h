/**
 * @file ultraSonic.h
 * @brief HC-SR04 ultrasonic distance sensor interface
 * @version 1.0.1
 * 
 * Measures distance using ultrasonic pulses.
 * Effective range: 2cm to 400cm
 * 
 * @author Michael Garcia, M&E Design
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#pragma once
#include <Arduino.h>

class UltraSonic
{
public:
    /**
     * @brief Initialize ultrasonic sensor pins
     */
    void begin();
    
    /**
     * @brief Read distance measurement
     * @return Distance in centimeters (0 if timeout/error)
     */
    int readCM();

private:
    static const uint8_t trigPin = 5;  // Trigger pin on D5
    static const uint8_t echoPin = 6;  // Echo pin on D6
};
