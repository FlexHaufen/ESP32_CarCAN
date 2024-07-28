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
#include <Adafruit_ILI9341.h>
#include <SD.h>


// ** DEFINES ***
// SPI - Using Hardware SPI on ESP32
#define SPI_CLK_FREQ        10000000U
// #define SPI_MOSI     23
// #define SPI_MISO     19
// #define SPI_SCK      18
// #define SPI_SS       5
#define SPI_DC              21
#define SPI_RST             22
#define SPI_DISPLAY_CS      2       // cs for display
#define SPI_SD_CS           4       // cs for sd card

// *** GLOBAL VARIABLES ***


class DisplayHandler {
public:

    DisplayHandler()
    : m_tft(Adafruit_ILI9341(SPI_DISPLAY_CS, SPI_DC, SPI_RST)) {}

    /**
     * @brief Init display handler
     * 
     */
    void Init();

    /**
     * @brief Update function
     * 
     */
    void OnUpdate();

private:
    void DrawBackground();

    void DrawMenuPoint(const char *str, int16_t x, int16_t y);

    void DrawMenuPointValue(uint16_t pid, int16_t x, int16_t y, const char *unit);

    void DrawMenuPointError(uint16_t pid, int16_t x, int16_t y);

   /**
    * @brief Read given file from SD card and draw it on screen
    * 
    * @param filename   file on SD card
    * @param x          x pos
    * @param y          y pos
    */
    void drawBMP(const char *filename, int16_t x, int16_t y);

private:
    // ** Members **
    Adafruit_ILI9341 m_tft;     // tft display
};
