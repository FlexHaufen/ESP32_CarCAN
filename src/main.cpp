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
#include <CAN.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// internal
//#include "CAN/CanHacker.h"
//#include "CAN/CanHackerLineReader.h"



// ** DEFINES ***
// SPI
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK  18
#define SPI_CS   5
#define SPI_DC   21
#define SPI_RST  22

// *** GLOABL VARIABLES ***
//CanHackerLineReader *lineReader = NULL;
//CanHacker *canHacker = NULL;

// Using Serial for USB communication
SoftwareSerial SerialUSB; // or HardwareSerial SerialUSB(2);


void readMessage(void);


/**
 * @brief Setup
 * 
 */
void setup() {
    Serial.begin(115200);
    SerialUSB.begin(115200);
    
    //Stream *interfaceStream = &Serial;
    //Stream *debugStream = &softwareSerial;
    //
    //
    //canHacker = new CanHacker(interfaceStream, debugStream, SPI_CS_PIN);
    ////canHacker->enableLoopback(); // uncomment this for loopback
    //lineReader = new CanHackerLineReader(canHacker);

    CAN.setPins(4, 2);
    CAN.begin(500000);

}

/**
 * @brief Loop
 * 
 */
void loop() {

    // Check if CAN data is available to read
    if (CAN.parsePacket()) {
        // If data is available, print it to serial
        while (CAN.available()) {
            Serial.print("t");
            Serial.print(CAN.packetId(), HEX);
            for (int i = 0; i < CAN.packetDlc(); i++) {
                Serial.print(CAN.read(), HEX);
                Serial.print(" ");
            }
            Serial.println();
        }
    }
}

/*
// Create a function to read message and display it through Serial Monitor
void readMessage() {
  unsigned long can_ID; // assign a variable for Message ID
  byte can_length; //assign a variable for length
  byte can_data[8] = {0}; //assign an array for data

  if (CAN.available() == true) // Check to see if a valid message has been received.
  {
    CAN.read(&can_ID, &can_length, can_data); // read Message and assign data through reference operator &

    Serial.print(millis());
    Serial.print(F(",0x"));
    Serial.print(can_ID, HEX); // Displays received ID
    Serial.print(',');
    Serial.print(can_length, HEX); // Displays message length
    for (byte i = 0; i < can_length; i++)
    {
      Serial.print(',');
      if (can_data[i] < 0x10) // If the data is less than 10 hex it will assign a zero to the front as leading zeros are ignored...
      {
        Serial.print('0');
      }
      Serial.print(can_data[i], HEX); // Displays message data
    }
    Serial.println(); // adds a line
  }
}
*/