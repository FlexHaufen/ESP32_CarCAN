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
#include "log.h"
#include "DisplayHandler/DisplayHandler.h"


DisplayHandler display;

void setup() {
 
    LogInit();
    display.Init();
}

void loop() {
    
    display.OnUpdate();
    delay(10);
}