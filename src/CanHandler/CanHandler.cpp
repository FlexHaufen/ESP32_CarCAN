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
Ticker g_TxTimer;
Ticker g_RxTimer;

// *** PROTOTYPES ***
void CanHandlerUpdate();

void HandleTxEvent();
void HandleRxEvent();



void CanHandlerInit() {
    // inizialize can
    CAN0.setCANPins(CAN_PIN_RX,CAN_PIN_TX);
    if (!CAN0.begin(500000)) {
        ESP.restart();
    }
    CAN0.watchFor(); 

    // Setup Timers
    g_TxTimer.attach_ms(CAN_TIMER_TX_PERIOD, HandleTxEvent);
    g_RxTimer.attach_ms(CAN_TIMER_RX_PERIOD, HandleRxEvent);
}


void HandleTxEvent() {
    for (auto& [pid, data] : g_OBD2Data) {
        if (data.txFlag == false) {
            // Prepare can frame
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
            // Send request
            CAN0.sendFrame(txFrame);
            
            // Update flags
            data.txFlag = true;
            data.timeOutCnt = 0;
        }
        else {
            data.timeOutCnt++;
        }

        // TODO: Error Handling here
        if (data.timeOutCnt >= CAN_MAX_TIMEOUT) {
            Serial.print("ERROR: No awnser from PID ");
            Serial.println(pid);
        }
    }
}

void HandleRxEvent() {
    CAN_FRAME rxFrame;
    if (!CAN0.read(rxFrame))  {
        return;
    }
    if (rxFrame.id != OBD2_CAN_RX_ID_ECU) {
        return;
    }
    
    int16_t rawValue = rxFrame.data.uint8[3];

    switch (rxFrame.id) {
        case OBD2_PID_ENGINE_LOAD:
        case OBD2_PID_THROTTLE_POS:
            g_OBD2Data[rxFrame.id].data = (100/255) * rawValue;
            break;
        case OBD2_PID_ENGINE_TEMP:
        case OBD2_PID_INTAKE_TEMP:
            g_OBD2Data[rxFrame.id].data = rawValue - 40;
            break;
        default:
            g_OBD2Data[rxFrame.id].data = rawValue;
            break;
    }

    g_OBD2Data[rxFrame.id].txFlag = false;
}

void PrintData() {
    Serial.println("Data: ");
    for (auto& [pid, data] : g_OBD2Data) {
        Serial.print("[");
        Serial.print(pid, HEX);
        Serial.print("]: ");
        Serial.print(data.data);
        Serial.print(" - Tx: ");
        Serial.print(data.txFlag);
        Serial.print(" - Cnt: ");
        Serial.println(data.timeOutCnt);
    
    }
}