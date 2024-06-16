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
#include "CanHandler/CanHandler.h"

#include "log.h"


// *** GLOBAL VARIABLES ***
Ticker g_TxTimer;       // Tx Timer
bool g_IsConnected = false;

static std::map<uint16_t, obd2Data_t> g_OBD2Data {
    {OBD2_PID_ENGINE_LOAD,          {0, 0, 0}},
    {OBD2_PID_ENGINE_TEMP,          {0, 0, 0}},
    {OBD2_PID_INTAKE_TEMP,          {0, 0, 0}},
    {OBD2_PID_THROTTLE_POS,         {0, 0, 0}},
    {OBD2_PID_SPEED,                {0, 0, 0}},
    //{OBD2_PID_DISTANCE_SINCE_CLR,   {0, 0, 0}},
    {OBD2_PID_ECU_VOLTAGE,          {0, 0, 0}},
};

// *** PROTOTYPES ***
void HandleTxEvent();
void HandleRxEvent(CAN_FRAME* rxFrame);

void CanHandlerInit() {
    // initialize can

    LOG_INFO("Initializing CAN");

    CAN0.setCANPins(CAN_PIN_RX,CAN_PIN_TX);
    if (!CAN0.begin(500000)) {
        ESP.restart();
    }
    CAN0.watchFor(); 

    // Setup Timers
    g_TxTimer.attach_ms(CAN_TIMER_TX_PERIOD, HandleTxEvent);

    CAN0.setRXFilter(0, OBD2_CAN_RX_ID_ECU, 0xFFE , false);
    CAN0.attachCANInterrupt(0, HandleRxEvent);
}


// TODO: Maybe have different tasks with different cycle times
//       50ms cycle for important information
//       1s cycle for non important information
void HandleTxEvent() {
    static std::map<uint16_t, obd2Data_t>::iterator it = g_OBD2Data.begin();

    if (it == g_OBD2Data.end()) {
        it = g_OBD2Data.begin();
    }
    
    auto& pid = it->first;
    auto& data = it->second;

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
    data.timeOutCnt++;

    // TODO: Error Handling here
    if (data.timeOutCnt >= CAN_MAX_TIMEOUT) {
        data.hasNoRxError = true;
        LOG_ERROR("No answer from PID [0x%03X]", pid);
    }

    it++;
}

void HandleRxEvent(CAN_FRAME* rxFrame) {
    uint16_t pid = rxFrame->data.uint8[2];

    // Error if PID received for which request wasn't sent
    if (g_OBD2Data.find(pid) == g_OBD2Data.end()) {
        LOG_ERROR("Awnser from PID [0x%03X], but no request was sent");
        return;
    } 

    g_IsConnected = true;

    switch (pid) {
        case OBD2_PID_ENGINE_LOAD:
        case OBD2_PID_THROTTLE_POS:
            g_OBD2Data[pid].data = rxFrame->data.uint8[3] / 2.55f;
            break;
        case OBD2_PID_ENGINE_TEMP:
        case OBD2_PID_INTAKE_TEMP:
            g_OBD2Data[pid].data = rxFrame->data.uint8[3] - 40.f;
            break;
        case OBD2_PID_DISTANCE_SINCE_CLR:
            g_OBD2Data[pid].data = (255.f * rxFrame->data.uint8[3] 
                                          + rxFrame->data.uint8[4]);
            break;
        case OBD2_PID_ECU_VOLTAGE:
            g_OBD2Data[pid].data = (255.f * rxFrame->data.uint8[3] 
                                          + rxFrame->data.uint8[4]) / 1000.f;
            break;
        case OBD2_PID_ODOMETER:
            g_OBD2Data[pid].data = ((rxFrame->data.uint8[3] << 24)
                                   +(rxFrame->data.uint8[4] << 16)
                                   +(rxFrame->data.uint8[5] <<  8)
                                   +(rxFrame->data.uint8[6] <<  0));
            break;
        case OBD2_PID_SPEED:
        default:
            g_OBD2Data[pid].data = rxFrame->data.uint8[3];
            break;
    }
    g_OBD2Data[pid].timeOutCnt = 0;
    g_OBD2Data[pid].hasNoRxError = false;
}

float CanGetData(uint16_t pid) {
    try {
        return g_OBD2Data.at(pid).data;
    }
    catch (const std::out_of_range& e) {
        LOG_ERROR("PID [0x%03X] does not exist - returning 0");
        return 0;
    }
}

float CanGetOldData(uint16_t pid) {
    try {
        return g_OBD2Data.at(pid).oldData;
    }
    catch (const std::out_of_range& e) {
        LOG_ERROR("PID [0x%03X] does not exist - returning 0");
        return 0;
    }
}

void CanSetOldData(uint16_t pid, float data) {
    try {
        g_OBD2Data.at(pid).oldData = data;
    }
    catch (const std::out_of_range& e) {
        LOG_ERROR("PID [0x%03X] does not exist - returning");
        return;
    }
}

bool CanHasNoRxError(uint16_t pid) {
    try {
        return g_OBD2Data.at(pid).hasNoRxError;
    }
    catch (const std::out_of_range& e) {
        LOG_ERROR("PID [0x%03X] does not exist - returning 0");
        return 0;
    }
}