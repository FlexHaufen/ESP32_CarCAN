/**
 * @file WidgetStack.h
 * @author flexhaufen
 * @brief 
 * @version 0.1
 * @date 2024-07-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

// *** INCLUDE ***
#include <vector>
#include <algorithm>

#include "DisplayHandler/Widget/Widget.h"

// *** CLASS ***
class WidgetStack {
public:
	WidgetStack() = default;

	~WidgetStack() {
		for (Widget* Widget : m_Widgets) {
			Widget->OnDetach();
			delete Widget;
		}
	}

	void PushWidget(Widget* Widget) {
		m_Widgets.emplace(m_Widgets.begin() + m_WidgetInsertIndex, Widget);
		m_WidgetInsertIndex++;
	}

	void PopWidget(Widget* Widget) {
		auto it = std::find(m_Widgets.begin(), m_Widgets.begin() + m_WidgetInsertIndex, Widget);
		if (it != m_Widgets.begin() + m_WidgetInsertIndex) {
			Widget->OnDetach();
			m_Widgets.erase(it);
			m_WidgetInsertIndex--;
		}
	}

	std::vector<Widget*>::iterator begin() { return m_Widgets.begin(); }
	std::vector<Widget*>::iterator end() { return m_Widgets.end(); }
	std::vector<Widget*>::reverse_iterator rbegin() { return m_Widgets.rbegin(); }
	std::vector<Widget*>::reverse_iterator rend() { return m_Widgets.rend(); }

	std::vector<Widget*>::const_iterator begin() const { return m_Widgets.begin(); }
	std::vector<Widget*>::const_iterator end()	const { return m_Widgets.end(); }
	std::vector<Widget*>::const_reverse_iterator rbegin() const { return m_Widgets.rbegin(); }
	std::vector<Widget*>::const_reverse_iterator rend() const { return m_Widgets.rend(); }
private:
	std::vector<Widget*> m_Widgets;			// Widget Vector
	uint32_t m_WidgetInsertIndex = 0;
};