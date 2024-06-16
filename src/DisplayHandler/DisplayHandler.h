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
#include "log.h"
#include "CanHandler/CanHandler.h"
#include "bitMaps.h"

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
    : m_Display(Adafruit_ILI9341(SPI_CS, SPI_DC, SPI_MOSI, SPI_SCK, SPI_RST, SPI_MISO)) {}


    void Init() {
        LOG_INFO("Initializing display");

        m_Display.begin();

        DrawBackground();
        // TODO: Errorhandling in CANHandler
        // if no data is recived reset etc.
        // timeOurcnt
        CanHandlerInit();
    }

    void OnUpdate() {

        DrawMenuPointValue(OBD2_PID_ENGINE_TEMP, 140,  20, "C");    DrawMenuPointError(OBD2_PID_ENGINE_TEMP, 200,  20);
        DrawMenuPointValue(OBD2_PID_INTAKE_TEMP, 140,  60, "C");    DrawMenuPointError(OBD2_PID_INTAKE_TEMP, 200,  60);
        DrawMenuPointValue(OBD2_PID_ECU_VOLTAGE, 140, 100, "V");    DrawMenuPointError(OBD2_PID_ECU_VOLTAGE, 200, 100);


        //if (CanHandlerIsConnected()) {
            m_Display.drawBitmap(220, 300, image_link_bits, 15, 16, ILI9341_GREEN);
        //}

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

    void DrawMenuPointValue(uint16_t pid, int16_t x, int16_t y, const char *unit) {

        float data = CanGetData(pid);
        float oldData = CanGetOldData(pid);

        // Clear only if data changed
        if (data != oldData) {
            m_Display.setCursor(x, y);
            m_Display.setTextColor(ILI9341_BLACK);  m_Display.setTextSize(2);
            m_Display.printf("%.1f", oldData);
            m_Display.print(unit);
        }

        m_Display.setCursor(x, y);
        m_Display.setTextColor(ILI9341_RED);  m_Display.setTextSize(2);
        m_Display.printf("%.1f", data);
        m_Display.print(unit);

        // Update Data
        CanSetOldData(pid, data);
    }

    void DrawMenuPointError(uint16_t pid, int16_t x, int16_t y) {
        if (CanHasNoRxError(pid)) {
            m_Display.drawBitmap(x, y, image_crossed_bits, 11, 16, ILI9341_RED);
        }
        else {
            m_Display.drawBitmap(x, y, image_crossed_bits, 11, 16, ILI9341_BLACK);
        }
    }

private:
    // ** Members **
    Adafruit_ILI9341 m_Display;
};
