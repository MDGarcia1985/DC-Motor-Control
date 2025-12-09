/**
 * @file ultraSonic.cpp
 * @brief Implementation of HC-SR04 ultrasonic sensor
 * @version 2.0.0
 * 
 * Timing notes (per HC-SR04 spec and typical usage):
 *  - Trigger: 10µs HIGH pulse on TRIG
 *  - Module emits 8-cycle 40kHz burst
 *  - ECHO goes HIGH for duration of round-trip
 *  - Echo pulse width:
 *      ~100µs (very close) up to ~18ms (4m),
 *      ~36ms typical if no object detected
 *  - Allow >10ms after end of echo before next trigger.
 * 
 * This implementation:
 *  - Uses pulseIn() with a 30ms timeout
 *  - Enforces ≥60ms between triggers
 *  - Returns 0 on timeout or out-of-range values
 * 
 * @author Michael Garcia, M&E Design
 * @contact michael@mandedesign.studio
 * @website www.mandedesign.studio
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#include "ultraSonic.h"

void UltraSonic::begin()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
}

int UltraSonic::readCM()
{
    unsigned long nowUs = micros();

    // Enforce minimum interval between measurements.
    // If called faster (e.g., from a 10ms loop), just
    // return the last value without re-triggering.
    // Note: Subtraction handles micros() overflow correctly due to unsigned arithmetic
    if ((nowUs - lastMeasurementUs) < MIN_MEASUREMENT_INTERVAL_US)
    {
        return lastDistanceCm;
    }
    lastMeasurementUs = nowUs;

    // Ensure trigger is low before starting
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Send 10µs trigger pulse
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure echo pulse duration with timeout
    unsigned long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);

    // Timeout: no echo / out of range
    if (duration == 0)
    {
        lastDistanceCm = 0;
        return 0;
    }

    // Convert to distance: duration / 58 ≈ cm
    // amazonq-ignore-next-line
    int distanceCm = static_cast<int>(duration / 58UL);

    // Clamp to usable range (~2–400cm). Outside → treat as no valid reading.
    if (distanceCm < 2 || distanceCm > 400)
    {
        lastDistanceCm = 0;
        return 0;
    }

    lastDistanceCm = distanceCm;
    return distanceCm;
}
