/**
 * @file display.cpp
 * @brief Implementation of OLED display interface
 * @version 2.0.0
 * 
 * Renders a compact status view:
 *  - Title ("Motor Control")
 *  - Distance (cm) or "No Obstruction"
 *  - PWM percentage and direction (FWD/REV)
 *  - Error messages when sensor or state fails
 *  - Simple bar graph visualization of PWM magnitude
 * 
 * Uses U8g2 SSD1306 128x64 hardware I2C driver.
 * 
 * @author Michael Garcia, M&E Design
 * @contact michael@mandedesign.studio
 * @website www.mandedesign.studio
 * @copyright Copyright (c) 2025 Michael Garcia, M&E Design
 * @license MIT License
 */

#include "display.h"

Display::Display()
    : u8g2(U8G2_R0, U8X8_PIN_NONE)   // rotation, reset pin (none)
{
}

void Display::begin()
{
    u8g2.begin();
    u8g2.setFont(u8g2_font_ncenB08_tr);

    // Optional splash / clear
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "Motor Control");
    u8g2.drawStr(0, 24, "System Initializing...");
    u8g2.sendBuffer();
}

void Display::update(int distance, int pwmPercent, bool error)
{
    u8g2.clearBuffer();

    // Title
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Motor Control");

    if (error)
    {
        // Error view
        u8g2.drawStr(0, 30, "ERROR: Timeout");

        // amazonq-ignore-next-line
        char errStr[32];
        snprintf(errStr, sizeof(errStr), "Raw: %d cm", distance);
        u8g2.drawStr(0, 42, errStr);

        u8g2.drawStr(0, 54, "Check sensor/wiring");
    }
    else
    {
        // Distance line
        char distStr[32];
        if (distance <= 0)
        {
            // amazonq-ignore-next-line
            snprintf(distStr, sizeof(distStr), "No Obstruction");
        }
        else
        {
            snprintf(distStr, sizeof(distStr), "Distance: %d cm", distance);
        }
        u8g2.drawStr(0, 30, distStr);

        // PWM + direction line
        char pwmStr[32];
        if (pwmPercent < 0)
        {
            snprintf(pwmStr, sizeof(pwmStr), "PWM: %d%% REV", -pwmPercent);
        }
        else
        {
            snprintf(pwmStr, sizeof(pwmStr), "PWM: %d%% FWD", pwmPercent);
        }
        u8g2.drawStr(0, 42, pwmStr);

        // Status bar (abs magnitude, 0–100%)
        int mag = abs(pwmPercent);
        if (mag > 100) mag = 100;

        u8g2.drawFrame(0, 50, 128, 10);

        if (mag > 0)
        {
            // Map 0–100% to 0–124px inside the frame
            int barWidth = (mag * 124) / 100;
            u8g2.drawBox(2, 52, barWidth, 6);
        }
    }

    u8g2.sendBuffer();
}
