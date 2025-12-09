/**
 * @file ultraSonic.h
 * @brief HC-SR04 ultrasonic distance sensor interface
 * @version 2.1.0
 * 
 * Measures distance using ultrasonic pulses.
 * Effective range: ~2cm to 400cm.
 * Returns 0 if:
 *  - No echo is received within timeout, or
 *  - Distance is outside the valid range.
 * 
 * Internally enforces a minimum interval between
 * measurements to respect sensor timing constraints.
 * 
 * @author Michael Garcia, M&E Design
 * @contact michael@mandedesign.studio
 * @website www.mandedesign.studio
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
     * @return Distance in centimeters (0 if timeout/invalid)
     */
    int readCM();

private:
    static const uint8_t trigPin = 5;  ///< Trigger pin on D5
    static const uint8_t echoPin = 6;  ///< Echo pin on D6

    /// @brief Minimum time between measurements in microseconds (~60ms)
    static const unsigned long MIN_MEASUREMENT_INTERVAL_US = 60000UL;

    /// @brief Timeout for echo pulse in microseconds (~30ms)
    /// (Echo is ~36ms max when no object; we bail a bit earlier)
    static const unsigned long ECHO_TIMEOUT_US = 30000UL;

    /// @brief Timestamp of last measurement (micros())
    unsigned long lastMeasurementUs = 0;

    /// @brief Last valid distance reading (cm), 0 if none
    int lastDistanceCm = 0;
};
