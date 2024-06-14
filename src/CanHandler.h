/**
 * @file CanHandler.h
 * @author flexhaufen
 * @brief 
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

// *** DEFINES ***

// OBD2-PID's & CAN-ID's according to https://en.wikipedia.org/wiki/OBD-II_PIDs
#define OBD2_CAN_TX_ID                  0x7DF
#define OBD2_CAN_RX_ID_ECU              0x7E8

#define OBD2_PID_FULE_SYSTEM_STATE      0x03
#define OBD2_PID_ENGINE_LOAD            0x04        // (100/255)A
#define OBD2_PID_ENGINE_TEMP            0x05        // A - 40
#define OBD2_PID_INTAKE_TEMP            0x0F        // A - 40
#define OBD2_PID_THROTTLE_POS           0x11        // (100/255)A



/**
 * @brief CanHandler for ESP32
 * 
 * @warning Serial must be initialized
 */
void CanHandlerInit();

void CanHandlerUpdate();

void PrintData();