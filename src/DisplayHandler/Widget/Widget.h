/**
 * @file Widget.h
 * @author flexhaufen
 * @brief 
 * @version 0.1
 * @date 2024-07-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

// *** INCLUDES ***
#include <string>


// *** CLASS ***
/**
 * @brief Widget class
 * 
 */
class Widget {
public:
	Widget(const std::string& name = "Widget")
	: m_Name(name) {}
	virtual ~Widget() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() = 0;

	const std::string& GetName() const { return m_Name; }
protected:
	std::string m_Name;	// Name of widget

private:

	uint16_t x, y;	// pos of widget

};