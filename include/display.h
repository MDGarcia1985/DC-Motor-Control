/**
 * @file display.h
 * @brief OLED display interface using SSD1306 128x64
 * @version 2.1.0
 * 
 * Displays system status including:
 * - Distance measurement from ultrasonic sensor
 * - Motor PWM output percentage and direction
 * - Error codes and status messages
 * 
 * Uses I2C communication on Arduino Uno R3 SDA/SCL pins (AD4/AD5)
 * 
 * @author Michael Garcia, M&E Design
 * @contact michael@mandedesign.studio
 * @website www.mandedesign.studio
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#pragma once
#include <Arduino.h>
#include <U8g2lib.h>

class Display
{
public:
    /**
     * @brief Constructor
     */
    Display();
    
    /**
     * @brief Initialize OLED display
     */
    void begin();
    
    /**
     * @brief Update display with current system status
     * @param distance Distance reading in cm (0 = no obstruction / invalid)
     * @param pwmPercent Motor PWM output (-100 to 100, sign = direction)
     * @param error Error state flag (true = sensor/system fault)
     */
    void update(int distance, int pwmPercent, bool error);

private:
    /// @brief SSD1306 128x64 OLED driver (hardware I2C, full buffer)
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
};
