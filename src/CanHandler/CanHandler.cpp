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


// *** GLOBAL VARIABLES ***
Ticker g_Timer_100ms;


// *** PROTOTYPES ***
void CanHandlerUpdate();


void CanHandlerInit() {
    // inizialize can
    CAN0.setCANPins(CAN_PIN_RX,CAN_PIN_TX);
    if (!CAN0.begin(500000)) {
        ESP.restart();
    }
    CAN0.watchFor(); 

    g_Timer_100ms.attach(1, CanHandlerUpdate);
}

void RequestData(uint16_t pid) {
    CAN_FRAME txFrame;
    txFrame.id = OBD2_CAN_TX_ID;
    txFrame.length = 8;
    txFrame.data.uint8[0] = 0x02;                   // Number of additional data bytes
    txFrame.data.uint8[1] = OND2_CAN_TX_SERVICE_01; // Service 01
    txFrame.data.uint8[2] = pid;                    // PID for information
    txFrame.data.uint8[3] = 0x00;                   // Padding byte
    txFrame.data.uint8[4] = 0x00;                   // Padding byte
    txFrame.data.uint8[5] = 0x00;                   // Padding byte
    txFrame.data.uint8[6] = 0x00;                   // Padding byte
    txFrame.data.uint8[7] = 0x00;                   // Padding byte
    CAN0.sendFrame(txFrame);
}

int16_t ReciveData(uint16_t pid) {
    CAN_FRAME rxFrame;
    if (!CAN0.read(rxFrame))  {
        Serial.println("WARNING No Data recived");
        return -1;
    }
    if (rxFrame.id != OBD2_CAN_RX_ID_ECU) {
        //Serial.println("WARNING PID outside range");
        return -1;
    }
    return rxFrame.data.uint8[3];
}

void CanHandlerUpdate() {
    for (auto& [pid, value] : g_OBD2Data) {
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
    for (auto& [pid, value] : g_OBD2Data) {
        Serial.print("[");
        Serial.print(pid, HEX);
        Serial.print("]: ");
        Serial.println(value);
    }
}