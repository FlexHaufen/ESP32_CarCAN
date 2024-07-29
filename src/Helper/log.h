/**
 * @file log.h
 * @author flexhaufen
 * @brief Simple logger library
 * @version 0.1
 * @date 2024-06-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

// *** INCLUDE ***
#include <Arduino.h>


// *** DEFINES ***

#define LOG_ENABLE

#ifdef LOG_ENABLE
    #define LOG_ERROR(...)     Serial.print("[ERROR]\t");   Serial.printf(__VA_ARGS__); Serial.print("\n")
    #define LOG_WARN(...)      Serial.print("[WARNING]\t"); Serial.printf(__VA_ARGS__); Serial.print("\n")
    #define LOG_INFO(...)      Serial.print("[INFO]\t");    Serial.printf(__VA_ARGS__); Serial.print("\n")
#else
    #define LOG_ERROR(...) 
    #define LOG_WARN(...)  
    #define LOG_INFO(...)  
#endif


// *** FUNCTIONS ***
static void LogInit() {
    Serial.begin(115200);
}