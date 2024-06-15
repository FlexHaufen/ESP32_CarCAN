/**
 * @file DisplayHandler.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

// *** INCLUDES ***
// external
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// internal
#include "CanHandler/CanHandler.h"

// ** DEFINES ***
// SPI
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK  18
#define SPI_CS   2
#define SPI_DC   21
#define SPI_RST  22

#define DISPLAY_WIDTH       240 // [px]
#define DISPLAY_HEIGHT      320 // [px]

// *** GLOBAL VARIABLES ***


class DisplayHandler {
public:

    DisplayHandler()
    : m_Display(Adafruit_ILI9341(SPI_CS, SPI_DC, SPI_MOSI, SPI_SCK, SPI_RST, SPI_MISO)) {
    }

    void ReadDiagnostics() {
        // read diagnostics (optional but can help debug problems)
        Serial.println("ILI9341 Debug Indormation"); 
        Serial.print(" - Display Power Mode: 0x"); Serial.println( m_Display.readcommand8(ILI9341_RDMODE), HEX);
        Serial.print(" - MADCTL Mode: 0x"); Serial.println(m_Display.readcommand8(ILI9341_RDMADCTL), HEX);
        Serial.print(" - Pixel Format: 0x"); Serial.println(m_Display.readcommand8(ILI9341_RDPIXFMT), HEX);
        Serial.print(" - Image Format: 0x"); Serial.println(m_Display.readcommand8(ILI9341_RDIMGFMT), HEX);
        Serial.print(" - Self Diagnostic: 0x"); Serial.println(m_Display.readcommand8(ILI9341_RDSELFDIAG), HEX); 
    }

    void Init() {
        m_Display.begin();

        DrawBackground();
        //CanHandlerInit();
    }

    void OnUpdate() {

        DrawMenuValue(OBD2_PID_ENGINE_TEMP, 140, 20, "C");
        DrawMenuValue(OBD2_PID_INTAKE_TEMP, 140, 60, "C");
        //DrawMenuValue(OBD2_PID_ENGINE_TEMP, 140, 20, "C");


        DrawMenuValue(OBD2_PID_DISTANCE_SINCE_CLR, 20, 275, "km");
        DrawMenuValue(OBD2_PID_ODOMETER, 20, 300, "km");
    }

private:
    void DrawBackground() {
        m_Display.fillScreen(ILI9341_BLACK);
        DrawMenuPoint("ENG T.", 20 ,  20);
        DrawMenuPoint("AIR T.", 20 ,  60);
        DrawMenuPoint("BAT V.", 20 , 100);
    }

    void DrawMenuPoint(const char *str, int16_t x, int16_t y) {
        m_Display.fillRoundRect(x - 2, y - 2, 80, 18, 2, ILI9341_RED);
        m_Display.setCursor(x, y);
        m_Display.setTextColor(ILI9341_BLACK);  m_Display.setTextSize(2);
        m_Display.print(str);
    }

    void DrawMenuValue(uint16_t pid, int16_t x, int16_t y, const char *unit) {
        if (GetOBD2Data().at(pid).data !=  GetOBD2Data().at(pid).oldData) {
            m_Display.setCursor(x, y);
            m_Display.setTextColor(ILI9341_BLACK);  m_Display.setTextSize(2);
            m_Display.print(GetOBD2Data().at(pid).oldData);
            m_Display.print(unit);
        }

        m_Display.setCursor(x, y);
        m_Display.setTextColor(ILI9341_RED);  m_Display.setTextSize(2);
        m_Display.print(GetOBD2Data().at(pid).data);
        m_Display.print(unit);

        GetOBD2Data().at(pid).UpdateOldData();
    }

private:
    // ** Members **
    Adafruit_ILI9341 m_Display;
};
