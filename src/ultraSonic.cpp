/**
 * @file ultraSonic.cpp
 * @brief Implementation of HC-SR04 ultrasonic ranging (non-blocking, rate-limited)
 * @version 3.0.0
 *
 * Timing notes (per HC-SR04 spec and field best practices):
 *  - Trigger: ≥10µs HIGH pulse causes an 8-cycle 40kHz acoustic burst
 *  - Echo signal:
 *       HIGH duration encodes round-trip travel time
 *       Reflex distance proportional to speed of sound (~343 m/s)
 *
 * Echo pulse window behavior:
 *  - Very near (<2cm): 100–250µs pulses
 *  - Mid-range (20–200cm): several milliseconds
 *  - Far objects up to sensor max (400cm): ~18ms
 *  - No return or open space: ~36ms worst case
 *
 * Device pacing requirement:
 *  - Ultrasonic bursts should be separated by ≥50ms
 *  - Faster triggering causes overlapping reflections
 *  - Wave interference = unstable readings, "runaway" values,
 *    or long random timeouts
 *
 * Implementation strategy (v3.0.0):
 *  - Trigger logic is scheduled: LOW→2µs→HIGH→10µs→LOW
 *  - Echo measured using pulseIn() with a 30ms timeout
 *  - If readCM() is called too soon (<MIN_INTERVAL_MS),
 *    the class returns the last stable value rather than forcing a false echo
 *  - Valid measurements are integer-converted using:
 *        distance_cm = duration_us / 58
 *    (which is numerically equivalent to:
 *        distance_cm = (duration * 0.000001 * 343 * 100) / 2
 *     but integer math is faster and AVR-friendly)
 *
 * Validity filtering:
 *  - Distances <2cm or >400cm are treated as invalid → returns 0
 *  - Timeout (pulseIn returns 0) → treated as 0
 *
 * Return semantics:
 *  - distance == 0 does NOT strictly mean "physical zero distance"
 *  - It means:
 *        a) no obstruction,
 *        b) beyond sensor range,
 *        c) echo timing invalid,
 *        d) noise-induced timeout,
 *        e) sensor out of angular field
 *  - Downstream logic interprets 0 as “no obstruction / no threat”
 *
 * Non-blocking design goals:
 *  - No delay() anywhere
 *  - No locking behavior
 *  - Safe in closed-loop real-time motion control
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

#include "ultraSonic.h"

void UltraSonic::begin()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
}

int UltraSonic::readCM()
{
    unsigned long now = millis();

    // Enforce rate limiting: if called too soon, don't re-trigger
    if (now - lastReadMs < MIN_INTERVAL_MS)
        return lastDistance;

    lastReadMs = now;

    // --- Trigger the sensor burst ---
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // --- Measure echo width with timeout ---
    unsigned long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);

    // Timeout or no echo
    if (duration == 0)
    {
        lastDistance = 0;
        return 0;
    }

    // Convert to centimeters using integer math
    int dist = static_cast<int>(duration / 58UL);

    // Range filter
    if (dist < 2 || dist > 400)
    {
        lastDistance = 0;
        return 0;
    }

    lastDistance = dist;
    return dist;
}
