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

#include "CanHandler/CanHandler.h"
#include "Helper/log.h"
#include "Helper/sd_read.h"

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

    if (!SD.begin(SPI_SD_CS, SPI, SPI_CLK_FREQ)) {
        LOG_ERROR("SD-Card mount failed!");
        return;
    } 

    drawBMP("/logo_animation30.bmp", 0, 0);

    // TODO: Errorhandling in CANHandler
    // if no data is recived reset etc.
    // timeOurcnt
    //CanHandlerInit();

}

void DisplayHandler::OnUpdate() {
    for (Widget* w : m_WidgetStack) {
        w->OnUpdate();
    }
}

// private
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
    if (sd_read::read16(bmpFS) != 0x4D42) {      // BMP signature
        LOG_ERROR("Given file is not a valid BMP file");
        return;
    }
    (void)sd_read::read32(bmpFS);            // BMP filesize         skip
    (void)sd_read::read32(bmpFS);            // BMP creator bytes    skip
    offset = sd_read::read32(bmpFS);         // BMP offset
    (void)sd_read::read32(bmpFS);            // BMP header size      skip
    w = sd_read::read32(bmpFS);              // BMP width
    h = sd_read::read32(bmpFS);              // BMP height
    // if BMP is uncompressed 24-Bit
    if ((sd_read::read16(bmpFS) != 1) && (sd_read::read16(bmpFS) != 24) && (sd_read::read32(bmpFS) != 0)) {
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

    for (uint16_t row = 0; row < h; row++) {
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