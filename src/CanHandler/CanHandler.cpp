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
Ticker g_TxTimer;       // Tx Timer

// *** PROTOTYPES ***
void HandleTxEvent();
void HandleRxEvent(CAN_FRAME* rxFrame);

void CanHandlerInit() {
    // inizialize can
    CAN0.setCANPins(CAN_PIN_RX,CAN_PIN_TX);
    if (!CAN0.begin(500000)) {
        ESP.restart();
    }
    CAN0.watchFor(); 

    // Setup Timers
    g_TxTimer.attach_ms(CAN_TIMER_TX_PERIOD, HandleTxEvent);

    CAN0.setRXFilter(0, OBD2_CAN_RX_ID_ECU, 0xFFF , false);
    CAN0.attachCANInterrupt(0, HandleRxEvent);
}


// TODO: Maby have diffrent tasks with diffrenc cycle times
//       50ms cycle for importand information
//       1s cycle for non importand information
void HandleTxEvent() {

    static std::map<uint16_t, obd2Data_t>::iterator it = g_OBD2Data.begin();

    if (it == g_OBD2Data.end()) {
        it = g_OBD2Data.begin();
    }
    
    auto& pid = it->first;
    auto& data = it->second;

    Serial.print("sending pid: ");
    Serial.println(pid);

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

    it++;
}

void HandleRxEvent(CAN_FRAME* rxFrame) {
    Serial.print("rx on id: ");
    Serial.println(rxFrame->id);
    
    uint16_t pid = rxFrame->data.uint8[2];

    if (g_OBD2Data.find(pid) == g_OBD2Data.end()) {
        Serial.print("Pid: ");
        Serial.print(pid);
        Serial.print("NotFound");
        return;
    } 
    else {
        int16_t rawValue = rxFrame->data.uint8[3];
        switch (pid) {
            case OBD2_PID_ENGINE_LOAD:
            case OBD2_PID_THROTTLE_POS:
                g_OBD2Data[pid].data = rawValue / 2.55f;
                break;
            case OBD2_PID_ENGINE_TEMP:
            case OBD2_PID_INTAKE_TEMP:
                g_OBD2Data[pid].data = rawValue - 40;
                break;
            default:
                g_OBD2Data[pid].data = rawValue;
                break;
        }
        g_OBD2Data[pid].txFlag = false;
    }
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