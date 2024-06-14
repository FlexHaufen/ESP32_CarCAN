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

// internal
#include "CanHandler.h"



void setup() {
    Serial.begin(115200);

    CanHandlerInit();
}

void loop() {
    
    PrintData();
    delay(1000);
}