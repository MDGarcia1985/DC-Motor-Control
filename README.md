# DC Motor Control System

Arduino-based DC motor control system with ultrasonic distance sensing and RGB LED status indication.

## Overview

This project implements an intelligent motor control system that adjusts motor speed based on real-time distance measurements from an ultrasonic sensor. Visual feedback is provided through an RGB LED that displays a color gradient corresponding to motor speed.

## Features

- **Distance-Based Speed Control**: Motor speed automatically adjusts based on proximity
- **Bidirectional Motor Control**: Supports forward and reverse operation
- **Visual Status Indicator**: RGB LED displays speed via color gradient (Red → Yellow → Green)
- **OLED Display**: Real-time display of distance, PWM output, direction, and status bar
- **Reverse Operation**: Motor reverses when object detected within 10cm
- **Smooth Transitions**: Gradual color changes and special blinking effects
- **Real-Time Operation**: 100Hz update rate for responsive control

## Hardware Requirements

### Components
- Arduino Uno R3
- L293D motor driver IC
- DC motor (6-12V)
- HC-SR04 ultrasonic sensor
- SSD1306 OLED display (128x64, I2C)
- Common anode RGB LED
- 10kΩ resistor (for L293D EN pin)
- 330Ω resistors (3x, for RGB LED current limiting)
- 100µF polarized capacitor (power supply filtering)
- 100nF ceramic capacitors (2x, for L293D and motor noise suppression)
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
| **OLED Display (I2C)** | SDA | AD4 |
| | SCL | AD5 |

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

#### Display Class (`display.h/cpp`)
- SSD1306 OLED display interface via I2C (128x64)
- Shows distance measurement in centimeters or "No Obstruction"
- Displays PWM output percentage with direction (FWD/REV)
- Visual status bar for motor speed (uses absolute value)
- Optimized for low RAM usage

## Operation Logic

### Distance Zones

| Distance Range | Motor Speed | LED Color |
|----------------|-------------|-----------|
| < 10 cm | -50% (Reverse) | Red |
| 10-30 cm | 50% (Forward) | Yellow (Blinking) |
| > 30 cm | 100% (Forward) | Green |
| No Obstruction (0 cm) | 100% (Forward) | Green |

### Control Flow

1. Ultrasonic sensor measures distance every 10ms (100Hz update rate)
2. Distance value determines target motor speed and direction
3. Timeout (>38ms) returns 0 = no obstruction detected
4. Motor speed is applied via PWM (-100 to +100)
5. LED color smoothly transitions to match speed
6. OLED display updates with distance, PWM%, direction (FWD/REV), and status bar
7. Process repeats continuously

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
- VCC2 (Pin 8): External motor power (6-12V) with 100nF ceramic capacitor to GND
- GND (Pins 4, 5, 12, 13): Common ground
- EN1 (Pin 1): Tied HIGH with 10kΩ resistor
- IN1 (Pin 2): Arduino D9
- IN2 (Pin 7): Arduino D10
- OUT1 (Pin 3): Motor terminal 1
- OUT2 (Pin 6): Motor terminal 2

### Capacitor Placement
- 100µF polarized capacitor: Across motor power supply (+/- terminals)
- 100nF ceramic capacitor: VCC2 (Pin 8) to GND on L293D
- 100nF ceramic capacitor: Across motor terminals (noise suppression)

### RGB LED
- Red (D3): 330Ω current limiting resistor
- Green (D11): 330Ω current limiting resistor
- Blue (D2): 330Ω current limiting resistor
- Common cathode to GND

### OLED Display (I2C)
- SDA: Arduino AD4
- SCL: Arduino AD5
- VCC: 5V or 3.3V (check display specifications)
- GND: Common ground

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
| System freezes after 30-40s | RAM overflow | Reduce update rate or disable serial debugging |
| Display not working | I2C connection | Check SDA/SCL connections on AD4/AD5 |
| Display shows garbage | Wrong I2C address | Verify display uses 0x3C address |
| Erratic distance readings | Sensor wiring | Check TRIG/ECHO connections on D5/D6 |
| LED colors inverted | Common cathode LED | Change to non-inverted logic in led.cpp setRGB() |
| Motor runs one direction only | Wiring error | Verify IN1/IN2 connections on D9/D10 |
| Sensor always shows 0cm | Timeout/no echo | Check sensor power (5V) and orientation |

## Safety Notes

- Never exceed motor voltage rating
- Ensure proper heat dissipation for L293D
- Use 330Ω current-limiting resistors for RGB LED
- Install capacitors for noise suppression and stability
- Connect all grounds together
- Do not draw motor power from Arduino 5V pin

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Michael Garcia  
M&E Design  
Email: michael@mandedesign.studio  
Website: www.mandedesign.studio  
Copyright © 2025
