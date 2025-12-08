# DC Motor Control System

Arduino-based DC motor control system with ultrasonic distance sensing and RGB LED status indication.

## Overview

This project implements an intelligent motor control system that adjusts motor speed based on real-time distance measurements from an ultrasonic sensor. Visual feedback is provided through an RGB LED that displays a color gradient corresponding to motor speed.

## Features

- **Distance-Based Speed Control**: Motor speed automatically adjusts based on proximity
- **Bidirectional Motor Control**: Supports forward and reverse operation
- **Visual Status Indicator**: RGB LED displays speed via color gradient (Red → Yellow → Green)
- **Error Detection**: Blue LED blinks when sensor readings are invalid
- **Smooth Transitions**: Gradual color changes and special blinking effects
- **Real-Time Operation**: 100Hz update rate for responsive control

## Hardware Requirements

### Components
- Arduino Uno R3
- L293D motor driver IC
- DC motor (6-12V)
- HC-SR04 ultrasonic sensor
- Common cathode RGB LED
- 10kΩ resistor (for L293D EN pin)
- 220Ω resistors (3x, for RGB LED current limiting)
- Breadboard and jumper wires
- External power supply for motor (6-12V)

### Pin Configuration

| Component | Pin | Arduino Pin |
|-----------|-----|-------------|
| **L293D Motor Driver** | IN1 | D9 (PWM) |
| | IN2 | D10 (PWM) |
| | EN | Tied HIGH (10kΩ to VCC) |
| **HC-SR04 Ultrasonic** | TRIG | D5 |
| | ECHO | D6 |
| **RGB LED** | Red | D3 (PWM) |
| | Green | D11 (PWM) |
| | Blue | D2 (Error) |

## Software Architecture

### Class Structure

#### Motor Class (`motor.h/cpp`)
- Controls L293D motor driver via PWM
- Supports speed range: -100 (full reverse) to +100 (full forward)
- Automatic direction control based on sign

#### UltraSonic Class (`ultraSonic.h/cpp`)
- Interfaces with HC-SR04 sensor
- Returns distance in centimeters (2-400cm range)
- 30ms timeout for invalid readings

#### StatusLED Class (`led.h/cpp`)
- RGB LED control with smooth color transitions
- Color mapping: Red (0%) → Yellow (50%) → Green (100%)
- Special blinking effect for 50% → 0% transitions
- Error mode: Blue LED blinks at 500ms intervals

## Operation Logic

### Distance Zones

| Distance Range | Motor Speed | LED Color |
|----------------|-------------|-----------|
| < 10 cm | 0% (Stop) | Red |
| 10-30 cm | 50% (Medium) | Yellow |
| > 30 cm | 100% (Full) | Green |
| Invalid/Error | 0% (Stop) | Blue (Blinking) |

### Control Flow

1. Ultrasonic sensor measures distance every 10ms
2. Distance value determines target motor speed
3. Invalid readings trigger error mode (blue LED blinks, motor stops)
4. Motor speed is applied via PWM
5. LED color smoothly transitions to match speed
6. Process repeats continuously

## Building and Uploading

### PlatformIO

```bash
# Build project
pio run

# Upload to Arduino
pio run --target upload

# Monitor serial output (if needed)
pio device monitor
```

### Arduino IDE

1. Copy all `.h` files from `include/` to sketch folder
2. Copy all `.cpp` files from `src/` to sketch folder
3. Open `main.cpp` and rename to `.ino`
4. Select **Arduino Uno** as board
5. Upload to device

## Circuit Diagram Notes

### L293D Connections
- VCC1 (Pin 16): 5V from Arduino
- VCC2 (Pin 8): External motor power (6-12V)
- GND (Pins 4, 5, 12, 13): Common ground
- EN1 (Pin 1): Tied HIGH with 10kΩ resistor
- IN1 (Pin 2): Arduino D9
- IN2 (Pin 7): Arduino D10
- OUT1 (Pin 3): Motor terminal 1
- OUT2 (Pin 6): Motor terminal 2

### Power Supply
- Arduino powered via USB or barrel jack
- Motor requires separate power supply (6-12V)
- **Important**: Connect all grounds together (Arduino GND, motor supply GND, L293D GND)

## Customization

### Adjusting Distance Thresholds
Edit `main.cpp`:
```cpp
if (distance < 10)        // Change stop threshold
    speedPct = 0;
else if (distance < 30)   // Change medium speed threshold
    speedPct = 50;
```

### Modifying LED Colors
Edit `led.cpp` color mapping in the `update()` function.

### Changing Update Rate
Edit `main.cpp`:
```cpp
if (now - lastUpdate >= 10)  // Change from 10ms to desired interval
```

## Troubleshooting

| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| Motor doesn't run | Insufficient power | Use external power supply for motor |
| Blue LED blinking | Invalid sensor readings | Check TRIG/ECHO connections and sensor power |
| Erratic distance readings | Sensor wiring | Check TRIG/ECHO connections |
| LED not lighting | Wrong LED type | Ensure common cathode RGB LED |
| Motor runs one direction only | Wiring error | Verify IN1/IN2 connections |

## Safety Notes

- Never exceed motor voltage rating
- Ensure proper heat dissipation for L293D
- Use current-limiting resistors for RGB LED
- Connect all grounds together
- Do not draw motor power from Arduino 5V pin

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Michael Garcia  
M&E Design  
Copyright © 2025
