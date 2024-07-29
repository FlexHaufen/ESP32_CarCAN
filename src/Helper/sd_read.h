/**
 * @file DisplayHandler.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

// *** INCLUDE ***
#include <FS.h>

// *** NAMESPACE ***
namespace sd_read {

/**
 * @brief Read 16-bit types from the given file
 * 
 * @param f         File
 * @return uint16_t 16-Bit value
 */
uint16_t read16(fs::File &f) {
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

/**
 * @brief Read 32-bit types from the given file
 * 
 * @param f         File
 * @return uint32_t 32-Bit value
 */
uint32_t read32(fs::File &f) {
	uint32_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

}