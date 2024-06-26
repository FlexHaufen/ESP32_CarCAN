/**
 * @file CanHandler.h
 * @author flexhaufen
 * @brief   This file handles the can communication
 * @version 0.1
 * @date 2024-06-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

// *** INCLUDES ***
// external
#include <Arduino.h>
#include <Ticker.h>
#include <esp32_can.h>
#include <map>

// *** DEFINES ***

#define CAN_PIN_RX  GPIO_NUM_5      // Rx pin for TJA1051T/3
#define CAN_PIN_TX  GPIO_NUM_4      // Tx pin for TJA1051T/3

// TODO: find out fastest timing
#define CAN_TIMER_TX_PERIOD      1000 // [ms]
#define CAN_MAX_TIMEOUT            10

// OBD2-PID's & CAN-ID's according to https://en.wikipedia.org/wiki/OBD-II_PIDs
#define OBD2_CAN_TX_ID                  0x7DF       // CAN ID for OBD2 Tx
#define OND2_CAN_TX_SERVICE_01          0x01        

#define OBD2_CAN_RX_ID_ECU              0x7E8       // CAN ID for OBD2 Rx of ECU

#define OBD2_PID_FULE_SYSTEM_STATE      0x03        // TODO Implement

#define OBD2_PID_ENGINE_LOAD            0x04        // (100/255)A                       [%]
#define OBD2_PID_ENGINE_TEMP            0x05        // A - 40                           [°C]
#define OBD2_PID_INTAKE_TEMP            0x0F        // A - 40                           [°C]
#define OBD2_PID_THROTTLE_POS           0x11        // (100/255)A                       [%]
#define OBD2_PID_SPEED                  0x0D        // A                                [km/h]
#define OBD2_PID_DISTANCE_SINCE_CLR     0x31        // (256A + B)                       [km]
#define OBD2_PID_ECU_VOLTAGE            0x42        // (256A + B) / 1000
#define OBD2_PID_ODOMETER               0xA6        // (A(2^24)+B(2^16)+C(2^8)+D)/10    [km]

// *** GLOBAL VARIABLES ***


typedef struct {
    float data;             // OBD2 Data
    float oldData;          // OBD2 old Data
    uint8_t timeOutCnt;     // TimeOut cnt
    void UpdateOldData() { oldData = data; }
} obd2Data_t;


/**
 * @brief CanHandler for ESP32
 * 
 * @warning Serial must be initialized
 */
void CanHandlerInit();

/**
 * @brief Prints all the data in g_OBD2Data
 * 
 */
void PrintData();

/**
 * @brief Gets current obd2Data
 * 
 * @return std::map<uint16_t, obd2Data_t>& 
 */
std::map<uint16_t, obd2Data_t>& GetOBD2Data();


bool CanHandlerIsConnected();