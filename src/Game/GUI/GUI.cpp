/*
 * GUI.cpp
 *
 *  Created on: 12 lip 2013
 *      Author: mateusz
 */
#include "GUI.hpp"

using namespace GUI;

/** Wielce zaawansowany header ;> */
Mouse GUI::mouse;

bool GUI::isMouseCollision(float x, float y, float w, float h) {
	Vector<int>& m_pos = mouse.pos;
	return (m_pos.x > x && m_pos.x < x + w && m_pos.y > y && m_pos.y < y + h);
}

void Control::putCallback(usint type, Callback* callback) {
	callbacks.insert(pair<usint, Callback*>(type, callback));
}

void Control::catchEvent(const Event& event) {
	if (!border_enabled || !isMouseCollision(x, y, w, h)) {
		control_state = NORMAL;
		return;
	}
	switch (event.type) {
		case Event::MOUSE_PRESSED:
			control_state = CLICKED;
			break;

			/**
			 *
			 */
		case Event::MOUSE_RELEASED:
			control_state = NORMAL;
			break;
	}
	if (callbacks.find(event.type) != callbacks.end()
			&& callbacks[event.type]) {
		callbacks[event.type]->getCallback(this);
	}
}
