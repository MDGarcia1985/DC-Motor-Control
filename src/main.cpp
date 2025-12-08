/**
 * @file main.cpp
 * @brief DC Motor Control with Ultrasonic Distance Sensing
 * 
 * Controls motor speed based on ultrasonic distance measurements:
 * - Distance < 10cm: Motor stops (0%)
 * - Distance 10-30cm: Motor at medium speed (50%)
 * - Distance > 30cm: Motor at full speed (100%)
 * - Invalid reading: Error mode (motor stops, blue LED blinks)
 * 
 * RGB LED provides visual feedback:
 * - Normal: Color gradient (Red -> Yellow -> Green)
 * - Error: Blue LED blinks at 500ms intervals
 * 
 * @author Michael Garcia, M&E Design
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#include <Arduino.h>
#include "motor.h"
#include "ultraSonic.h"
#include "led.h"

// Hardware objects
Motor motor;
UltraSonic usonic;
StatusLED statusLed;

// State variables
unsigned long lastUpdate = 0;
int lastSpeedPct = 0;
bool error = false;

void setup()
{
    // Initialize all hardware components
    motor.begin();
    usonic.begin();
    statusLed.begin();
}

void loop()
{
    unsigned long now = millis();
    
    // Update at 100Hz (every 10ms)
    if (now - lastUpdate >= 10)
    {
        lastUpdate = now;
        
        // Read distance from ultrasonic sensor
        int distance = usonic.readCM();
        int speedPct = lastSpeedPct;
        
        // Update speed based on distance thresholds
        if (distance > 0 && distance <= 400)
        {
            error = false;
            
            if (distance < 10)
                speedPct = 0;      // Stop zone
            else if (distance < 30)
                speedPct = 50;     // Medium speed zone
            else
                speedPct = 100;    // Full speed zone
            
            lastSpeedPct = speedPct;
        }
        else
        {
            // Invalid reading: set error mode
            error = true;
            speedPct = 0;  // Stop motor on error
        }
        
        // Apply speed to motor and update LED indicator
        statusLed.setError(error);
        motor.setSpeed(speedPct);
        statusLed.update(speedPct);
    }
}
