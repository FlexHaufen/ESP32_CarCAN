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
#include "DisplayHandler/DisplayHandler.h"


DisplayHandler display;

void setup() {
    Serial.begin(115200);
    display.Init();
    display.ReadDiagnostics();
}

void loop() {
    
    display.OnUpdate();
    delay(10);
}