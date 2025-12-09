/**
 * @file ultraSonic.h
 * @brief HC-SR04 ultrasonic distance sensor interface (non-blocking, rate-limited)
 * @version 3.0.0
 * 
 * HC-SR04 operating summary:
 *  - TRIG receives a ≥10µs HIGH pulse to initiate a ranging cycle.
 *  - Module emits an 8-cycle 40kHz ultrasonic burst.
 *  - ECHO pin goes HIGH for a duration equal to round-trip flight time.
 * 
 * Echo pulse timing behavior:
 *  - Very short objects produce ~100µs ECHO width
 *  - Near objects: 1–5ms typical
 *  - Mid-range: ~10–18ms for values within 2m–4m
 *  - No echo or beyond sensor range: up to ~36ms maximum before timeout
 *
 * Sensor pacing requirement:
 *  - Avoid triggering faster than ~10–20Hz (i.e. ≥50ms interval)
 *  - If triggered too frequently, reflections from prior pings overlap
 *  - Overlapping echoes produce unstable, jittery, or runaway readings
 *
 * This implementation (v3.0.0):
 *  - Uses pulseIn() with a 30ms timeout (ECHO_TIMEOUT_US)
 *  - Rate-limits to ≥50ms between measurements (MIN_INTERVAL_MS)
 *  - Returns last stable value if polled faster
 *  - Converts valid echo durations to cm using integer-friendly 1/58 scaling
 *  - Converts invalid, out-of-range, or timeout readings to distance = 0
 *  - Distance = 0 means:
 *        "no obstruction" OR "timeout / invalid / beyond useful range"
 * 
 * Design intent:
 *  - Class is **non-blocking**, **safe**, and **deterministic**
 *  - Guarantees NO deadlocks and NO forced delays in real-time control logic
 *  - Intended for continuous feedback loops, motion control, and sensor fusion
 *
 * Pin mapping (Arduino Uno R3):
 *  - TRIG → D5  (digital output)
 *  - ECHO → D6  (digital input)
 *
 * Notes on electrical / wiring:
 *  - Sensor VCC must be 5V
 *  - Ground must be shared with main controller and motor subsystem
 *  - Long cables increase ringing and degrade accuracy
 *  - Keep sensor physically isolated from high-current motor leads if possible
 *
 * Integration context:
 *  - This class feeds a dynamic PWM controller
 *  - Safe for frequent main-loop polling (100Hz)
 *  - Internally returns the **most recent stable value** when polled too fast
 *  - Guarantees clean scaling into LED, motor, and OLED logic
 *
 * Author:
 *  - Michael Garcia, M&E Design
 *  - michael@mandedesign.studio
 *  - www.mandedesign.studio
 *
 * License:
 *  - Copyright (c) 2025
 *  - MIT License
 */

#pragma once
#include <Arduino.h>

class UltraSonic
{
public:
    void begin();

    /**
     * @brief Acquire distance measurement in centimeters.
     * 
     * @return int
     *   - >= 2 and <= 400: valid distance measurement
     *   - == 0: timeout / invalid / beyond range / "no obstruction"
     */
    int readCM();

private:
    static const uint8_t trigPin = 5;   ///< TRIG on D5
    static const uint8_t echoPin = 6;   ///< ECHO on D6

    /// Maximum echo wait before pulseIn gives up (µs)
    static const unsigned long ECHO_TIMEOUT_US = 30000UL; // 30 ms

    /// Minimum time between trigger events (ms)
    static const unsigned long MIN_INTERVAL_MS = 50UL; // ~20 Hz max

    /// Timestamp of last completed measurement
    unsigned long lastReadMs = 0;

    /// Last stable validated distance returned to caller
    int lastDistance = 0;
};
