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
#include <esp32_can.h>


CAN_FRAME can_message;
#define SERIAL_MAX_BUFF_LEN   30

typedef struct esp_frame_t {
  int mesh_id;
  unsigned long can_id;
  byte can_len;
  uint8_t can_data[8];
};

bool          send_can_msgs = true;
bool          send_timestamp = true;
bool          got_new_can_msg = false;
bool          send_OK = true;
uint8_t       buff[40];
char          slcan_com[SERIAL_MAX_BUFF_LEN];
esp_frame_t   esp_frame;


void sendCanOverSerial();
void handleSerial();
void send_ack();
void send_nack();
uint16_t  slcan_get_time();

void setup() {

    Serial.begin(115200);
    Serial.setTimeout(1);

    CAN0.setCANPins(GPIO_NUM_5, GPIO_NUM_4);
    CAN0.setListenOnlyMode(true);
    if (!CAN0.begin(500000)) {
        ESP.restart();
    }
    CAN0.watchFor();  
}

void loop() {
    
    if (CAN0.read(can_message)) {
        if (can_message.id == 0)  {
            return;
        }
        esp_frame.can_id = can_message.id;
        esp_frame.can_len = can_message.length;
        for (int i = 0; i < can_message.length; i++) {
            esp_frame.can_data[i] = can_message.data.byte[i];
        }
        got_new_can_msg = true;
    }
    sendCanOverSerial();
}

void sendCanOverSerial() {

    if (got_new_can_msg ) {
        char outputBuff[45]; // Increased size for safety
        int idx = 0;

        idx += snprintf(outputBuff + idx, sizeof(outputBuff) - idx, "t%03x", esp_frame.can_id);
        idx += snprintf(outputBuff + idx, sizeof(outputBuff) - idx, "%d", esp_frame.can_len);
        for (int i = 0; i < esp_frame.can_len; i++) {
            idx += snprintf(outputBuff + idx, sizeof(outputBuff) - idx, "%02x", esp_frame.can_data[i]);
        }

        if (send_timestamp) {
            uint16_t timestamp = slcan_get_time();
            idx += snprintf(outputBuff + idx, sizeof(outputBuff) - idx, "%04x", timestamp);
        }
        outputBuff[idx++] = '\r';     // Carriage return
        outputBuff[idx++] = '\0';    // Null-terminate the buffer
        Serial.print(outputBuff);
        got_new_can_msg = false;
    }
}

uint16_t  slcan_get_time() {
    return millis() % 0xEA60;   // Timespamp limit
}