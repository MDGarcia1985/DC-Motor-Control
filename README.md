# DC Motor Control System

Arduino-based DC motor control system with ultrasonic distance sensing and RGB LED status indication.

## Overview

This project implements an intelligent motor control system that dynamically adjusts motor speed based on real-time distance measurements from an ultrasonic sensor. Features smooth gradient speed control (5-60cm range), timed reverse maneuvers, and visual feedback through an RGB LED color gradient.

## Features

- **Dynamic Speed Mapping**: Smooth gradient from 0-100% across 5-60cm range with 2% quantization
- **Timed Reverse Maneuver**: Executes STOP (500ms) → REVERSE (200ms) → SLOW-FWD when distance ≥60cm
- **Rate-Limited Sensing**: 50ms minimum interval prevents echo overlap and unstable readings
- **Bidirectional Motor Control**: PWM-based forward/reverse with 3% deadband
- **Visual Status Indicator**: RGB LED displays speed via color gradient (Red → Yellow → Green)
- **OLED Display**: Real-time distance, PWM%, direction (FWD/REV), and status bar
- **Non-Blocking State Machine**: Fully millis()-based timing, no delay() calls
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

#### Motor Class (`motor.h/cpp`) v2.1.0
- Controls L293D motor driver via PWM on IN1/IN2
- Speed range: -100 (full reverse) to +100 (full forward)
- 3% deadband prevents audible whine from tiny PWM values
- Caches last command to avoid redundant pin writes
- Separate brake() method for active braking vs coasting

#### UltraSonic Class (`ultraSonic.h/cpp`) v3.0.0
- Interfaces with HC-SR04 sensor with rate limiting
- Enforces 50ms minimum between readings to prevent echo overlap
- Returns cached value when polled too fast
- 30ms timeout for invalid readings
- Returns distance in centimeters (2-400cm valid range, 0 = no obstruction)

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

### Dynamic Speed Mapping (v3.0.0)

| Distance Range | Motor Speed | LED Color | Mode |
|----------------|-------------|-----------|------|
| ≤ 5 cm | 0% (Stop) | Red | NORMAL |
| 5-60 cm | 0-100% (Gradient) | Red → Yellow → Green | NORMAL |
| ≥ 60 cm | Timed Maneuver | Varies | STOPPING → REVERSING → SLOW_FORWARD |
| 0 cm (No obstruction) | 100% (Forward) | Green | NORMAL |

### Timed Reverse Maneuver

When distance ≥ 60cm, system executes:
1. **STOPPING**: 0% for 500ms
2. **REVERSING**: -20% for 200ms
3. **SLOW_FORWARD**: +20% until valid reading < 60cm
4. Returns to NORMAL mode with dynamic mapping

### Control Flow

1. Main loop executes at 100Hz (10ms intervals)
2. Ultrasonic sensor reads distance (rate-limited to 50ms minimum)
3. State machine evaluates distance and current mode
4. Speed calculated via dynamic mapping (5-60cm → 0-100%) with 2% quantization
5. Motor speed applied via PWM with 3% deadband
6. LED color smoothly transitions to match speed
7. OLED display updates with distance, PWM%, direction, and status bar

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

### Adjusting Distance Mapping Range
Edit `main.cpp`:
```cpp
static const int MIN_DIST_CM = 5;    // distance where speed = 0%
static const int MAX_DIST_CM = 60;   // distance where speed = 100%
```

### Adjusting Maneuver Timing
Edit `main.cpp`:
```cpp
static const unsigned long STOP_TIME_MS    = 500; // stop duration
static const unsigned long REVERSE_TIME_MS = 200; // reverse duration
```

### Adjusting Motor Deadband
Edit `motor.h`:
```cpp
static const uint8_t DEAD_BAND_PERCENT = 3;  // minimum active speed
```

### Adjusting Sensor Rate Limit
Edit `ultraSonic.h`:
```cpp
static const unsigned long MIN_INTERVAL_MS = 50UL; // ~20 Hz max
```

### Modifying LED Colors
Edit `led.cpp` color mapping in the `update()` function.

### Changing Update Rate
Edit `main.cpp`:
```cpp
static const unsigned long UPDATE_INTERVAL_MS = 10;  // main loop pacing
```

## Troubleshooting

| Issue | Possible Cause | Solution |
|-------|----------------|----------|
| Motor doesn't run | Insufficient power | Use external power supply for motor |
| Unstable distance readings | Sensor polled too fast | Rate limiting enforced at 50ms minimum |
| Display not working | I2C connection | Check SDA/SCL connections on AD4/AD5 |
| Display shows garbage | Wrong I2C address | Verify display uses 0x3C address |
| Erratic distance readings | Sensor wiring | Check TRIG/ECHO connections on D5/D6 |
| LED colors inverted | Common cathode LED | Change to non-inverted logic in led.cpp setRGB() |
| Motor runs one direction only | Wiring error | Verify IN1/IN2 connections on D9/D10 |
| Sensor always shows 0cm | Timeout/no echo | Check sensor power (5V), orientation, and 50ms rate limit |
| Motor whines at low speed | PWM too low | 3% deadband automatically applied |

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
