/**
 * @file ultraSonic.cpp
 * @brief Implementation of HC-SR04 ultrasonic sensor
 * @version 1.0.1
 * 
 * @author Michael Garcia, M&E Design
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#include "ultraSonic.h"

void UltraSonic::begin()
{
    // Configure sensor pins
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

int UltraSonic::readCM()
{
    // Send 10μs trigger pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Measure echo pulse duration (timeout at 30ms)
    long duration = pulseIn(echoPin, HIGH, 30000);
    
    // Convert to distance: duration * speed_of_sound / 2
    // Speed of sound = 0.034 cm/μs
    return duration * 0.034 / 2;
}
