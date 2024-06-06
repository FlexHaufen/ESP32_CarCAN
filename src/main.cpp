/**
 * @file main.cpp
 * @author flexhaufen
 * @brief 
 * @version 0.1
 * @date 2024-06-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

// *** INCLUDES ***
// external
#include <Arduino.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// ** DEFINES ***
// SPI
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK  18
#define SPI_CS   5
#define SPI_DC   21
#define SPI_RST  22

// *** GLOABL VARIABLES ***
Adafruit_ILI9341 display = Adafruit_ILI9341(SPI_CS, SPI_DC, SPI_MOSI, SPI_SCK, SPI_RST, SPI_MISO);


/**
 * @brief Setup
 * 
 */
void setup() {
    Serial.begin(9600);
    
    display.begin();
    display.fillScreen(ILI9341_BLACK);

    // read diagnostics (optional but can help debug problems)
    Serial.println("ILI9341 Debug Indormation"); 
    Serial.print(" - Display Power Mode: 0x"); Serial.println( display.readcommand8(ILI9341_RDMODE), HEX);
    Serial.print(" - MADCTL Mode: 0x"); Serial.println(display.readcommand8(ILI9341_RDMADCTL), HEX);
    Serial.print(" - Pixel Format: 0x"); Serial.println(display.readcommand8(ILI9341_RDPIXFMT), HEX);
    Serial.print(" - Image Format: 0x"); Serial.println(display.readcommand8(ILI9341_RDIMGFMT), HEX);
    Serial.print(" - Self Diagnostic: 0x"); Serial.println(display.readcommand8(ILI9341_RDSELFDIAG), HEX); 
}

/**
 * @brief Loop
 * 
 */
void loop() {
    display.fillScreen(ILI9341_BLACK);
    display.setCursor(0, 0);
    display.setTextColor(ILI9341_WHITE);  display.setTextSize(1);
    display.println("Hello World!");
    display.setTextColor(ILI9341_YELLOW); display.setTextSize(2);
    display.println(1234.56);
    display.setTextColor(ILI9341_RED);    display.setTextSize(3);
    display.println(0xDEADBEEF, HEX);
    display.println();
    display.setTextColor(ILI9341_GREEN);
    display.setTextSize(5);
    display.println("Groop");
    display.setTextSize(2);
    display.println("I implore thee,");
    display.setTextSize(1);
    display.println("my foonting turlingdromes.");
    display.println("And hooptiously drangle me");
    display.println("with crinkly bindlewurdles,");
    display.println("Or I will rend thee");
    display.println("in the gobberwarts");
    display.println("with my blurglecruncheon,");
    display.println("see if I don't!");
    delay(1000);
}