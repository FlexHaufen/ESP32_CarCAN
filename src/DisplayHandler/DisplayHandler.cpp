/**
 * @file DisplayHandler.cpp
 * @author flexhaufen
 * @brief 
 * @version 0.1
 * @date 2024-07-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

// *** INCLUDES ***
#include "DisplayHandler/DisplayHandler.h"

#include "log.h"
#include "CanHandler/CanHandler.h"
#include "bitMap.h"

// *** CLASS ***

// public
void DisplayHandler::Init() {
    LOG_INFO("Initializing display");

    // avoid cs connection
    pinMode(SPI_SD_CS, OUTPUT);
    digitalWrite(SPI_SD_CS, HIGH);
    pinMode(SPI_DISPLAY_CS, OUTPUT);
    digitalWrite(SPI_DISPLAY_CS, HIGH);

    m_tft.begin(SPI_CLK_FREQ);
    m_tft.fillScreen(ILI9341_BLACK);

    if (!SD.begin(SPI_SD_CS)) {
        LOG_ERROR("SD-Card mount failed!");
        return;
    } 

    // FIXME: try other sd-card / or other spi speed
    drawBMP("/logo_animation30.bmp", 0, 0);

    //DrawBackground();
    // TODO: Errorhandling in CANHandler
    // if no data is recived reset etc.
    // timeOurcnt
    //CanHandlerInit();

}

void DisplayHandler::OnUpdate() {

    //DrawMenuPointValue(OBD2_PID_ENGINE_TEMP, 140,  20, "C");    DrawMenuPointError(OBD2_PID_ENGINE_TEMP, 200,  20);
    //DrawMenuPointValue(OBD2_PID_INTAKE_TEMP, 140,  60, "C");    DrawMenuPointError(OBD2_PID_INTAKE_TEMP, 200,  60);
    //DrawMenuPointValue(OBD2_PID_ECU_VOLTAGE, 140, 100, "V");    DrawMenuPointError(OBD2_PID_ECU_VOLTAGE, 200, 100);


    //if (CanHandlerIsConnected()) {
    //    m_tft.drawBitmap(220, 300, image_link_bits, 15, 16, ILI9341_GREEN);
    //}

}

// private

void DisplayHandler::DrawBackground() {
    m_tft.fillScreen(ILI9341_BLACK);
    DrawMenuPoint("ENG T.", 20 ,  20);
    DrawMenuPoint("AIR T.", 20 ,  60);
    DrawMenuPoint("BAT V.", 20 , 100);
}

void DisplayHandler::DrawMenuPoint(const char *str, int16_t x, int16_t y) {
    m_tft.fillRoundRect(x - 2, y - 2, 80, 18, 2, ILI9341_RED);
    m_tft.setCursor(x, y);
    m_tft.setTextColor(ILI9341_BLACK);  m_tft.setTextSize(2);
    m_tft.print(str);
}

void DisplayHandler::DrawMenuPointValue(uint16_t pid, int16_t x, int16_t y, const char *unit) {

    float data = CanGetData(pid);
    float oldData = CanGetOldData(pid);

    // Clear only if data changed
    if (data != oldData) {
        m_tft.setCursor(x, y);
        m_tft.setTextColor(ILI9341_BLACK);  m_tft.setTextSize(2);
        m_tft.printf("%.1f", oldData);
        m_tft.print(unit);
    }

    m_tft.setCursor(x, y);
    m_tft.setTextColor(ILI9341_RED);  m_tft.setTextSize(2);
    m_tft.printf("%.1f", data);
    m_tft.print(unit);

    // Update Data
    CanSetOldData(pid, data);
}

void DisplayHandler::DrawMenuPointError(uint16_t pid, int16_t x, int16_t y) {
    if (CanHasNoRxError(pid)) {
        m_tft.drawBitmap(x, y, bitMap::image_crossed_bits, 11, 16, ILI9341_RED);
    }
    else {
        m_tft.drawBitmap(x, y, bitMap::image_crossed_bits, 11, 16, ILI9341_BLACK);
    }
}


/**
* @brief Read given file from SD card and draw it on screen
* 
* @param filename   file on SD card
* @param x          x pos
* @param y          y pos
*/
void DisplayHandler::drawBMP(const char *filename, int16_t x, int16_t y) {
    if ((x >= m_tft.width()) || (y >= m_tft.height())) {
        LOG_ERROR("Given x & y are not in display boundaries");
        return; 
    }
    fs::File bmpFS = SD.open(filename);
    if (!bmpFS) {
        LOG_ERROR("Given file not found");
        return;
    }
    uint32_t offset = 0;            // offset to start of image data
    uint16_t w = 0, h = 0;          // width and height of image
    uint8_t  r = 0, g = 0, b = 0;   // rgb colors
    if (bitMap::read16(bmpFS) != 0x4D42) {      // BMP signature
        LOG_ERROR("Given file is not a valid BMP file");
        return;
    }
    (void)bitMap::read32(bmpFS);            // BMP filesize         skip
    (void)bitMap::read32(bmpFS);            // BMP creator bytes    skip
    offset = bitMap::read32(bmpFS);         // BMP offset
    (void)bitMap::read32(bmpFS);            // BMP header size      skip
    w = bitMap::read32(bmpFS);              // BMP width
    h = bitMap::read32(bmpFS);              // BMP height
    // if BMP is uncompressed 24-Bit
    if ((bitMap::read16(bmpFS) != 1) && (bitMap::read16(bmpFS) != 24) && (bitMap::read32(bmpFS) != 0)) {
        LOG_ERROR("BMP format not recognized.");
        return;
    }
    y += h - 1;             // Adjust y to start from the bottom (BMPs are stored bottom-to-top)
    bmpFS.seek(offset);     // Move file pointer to the start of image data
    uint16_t padding = (4 - ((w * 3) & 3)) & 3; // Calculate padding bytes at the end of each row
    uint16_t *lineBuffer = new uint16_t[w];     // Create a buffer to hold one row of pixel data
    if (!lineBuffer) {
        LOG_ERROR("Failed to allocate memory");
        bmpFS.close();
        return;
    }
    // Iterate over each row
    for (uint16_t row = 0; row < h; row++) {
        // Read the row of pixels
        for (uint16_t col = 0; col < w; col++) {
            b = bmpFS.read();
            g = bmpFS.read();
            r = bmpFS.read();
            lineBuffer[col] = m_tft.color565(r, g, b);
        }
        bmpFS.readBytes((char *)NULL, padding);
        m_tft.drawRGBBitmap(x, y - row, lineBuffer, w, 1);
    }
    // Skip padding bytes at the end of the row
    delete[] lineBuffer;
    bmpFS.close();
}
