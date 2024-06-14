/**
 * @file CanHandler.cpp
 * @author flexhaufen
 * @brief 
 * @version 0.1
 * @date 2024-06-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

// *** INCLUDES ***
#include "CanHandler.h"

#include <map>

std::map<uint16_t, int16_t> carData {
    {OBD2_PID_ENGINE_LOAD, 0},
    {OBD2_PID_ENGINE_TEMP, 0},
    {OBD2_PID_INTAKE_TEMP, 0},
    {OBD2_PID_THROTTLE_POS, 0}
};



Ticker m_Timer_100ms;


/**
 * @brief CanHandler for ESP32
 * 
 * @warning Serial must be initialized
 */
void CanHandlerInit() {

    // inizialize can
    CAN0.setCANPins(GPIO_NUM_5, GPIO_NUM_4);
    if (!CAN0.begin(500000)) {
        ESP.restart();
    }
    CAN0.watchFor(); 

    m_Timer_100ms.attach(1, CanHandlerUpdate);
}

void RequestData(uint16_t pid) {
    Serial.println("Requesting Data");
    CAN_FRAME txFrame;
    txFrame.id = 0x7DF;
    txFrame.length = 8;
    txFrame.data.uint8[0] = 0x02;  // Number of additional data bytes
    txFrame.data.uint8[1] = 0x01;  // Service 01
    txFrame.data.uint8[2] = pid;   // 
    txFrame.data.uint8[3] = 0x00;  // Padding byte
    txFrame.data.uint8[4] = 0x00;  // Padding byte
    txFrame.data.uint8[5] = 0x00;  // Padding byte
    txFrame.data.uint8[6] = 0x00;  // Padding byte
    txFrame.data.uint8[7] = 0x00;  // Padding byte
    CAN0.sendFrame(txFrame);
}

int16_t ReciveData(uint16_t pid) {
    Serial.println("Reciving Data");
    CAN_FRAME rxFrame;
    if (!CAN0.read(rxFrame))  {
        Serial.println("No Data recived");
        return -1;
    }
    if (rxFrame.id != 0x7E8) {
        Serial.println("PID outside range");
        return -1;
    }
    return rxFrame.data.uint8[3];
}

void CanHandlerUpdate() {
    for (auto& [pid, value] : carData) {
        RequestData(pid);
        int16_t rawValue = ReciveData(pid);
        switch (pid) {
            case OBD2_PID_ENGINE_LOAD:
            case OBD2_PID_THROTTLE_POS:
                value = (100/255) * rawValue;
                break;
            case OBD2_PID_ENGINE_TEMP:
            case OBD2_PID_INTAKE_TEMP:
                value = rawValue - 40;
                break;
            default:
                value = rawValue;
                break;
        }
    }
}


void PrintData() {
    Serial.println("Data: ");
    for (auto& [pid, value] : carData) {
        Serial.print("[");
        Serial.print(pid, HEX);
        Serial.print("]: ");
        Serial.println(value);
    }
}