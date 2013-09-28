/*
 * CheckBox.cpp
 *
 *  Created on: 27-09-2013
 *      Author: mateusz
 */
#include "GUI.hpp"

using namespace GUI;

Checkbox::Checkbox(const Rect<float>& _bounds, const char* _text,
		PlatformShape* _shape, bool _border_enabled, Callback* _callback) :
				Button(_bounds, _text, _shape, _border_enabled, _callback) {

	text_visible = false;
	invert_border = true;
	fill_type = SHARP;

	w = h = 32;
}

void Checkbox::catchEvent(const Event& event) {
	if (!isMouseCollision(x, y, w, h)) {
		return;
	}
	switch (event.type) {
		/**
		 *
		 */
		case Event::MOUSE_CLICKED:
			control_state = control_state == CLICKED ? NORMAL : CLICKED;
			break;
	}

	if (callbacks.find(event.type) != callbacks.end()
			&& callbacks[event.type]) {
		callbacks[event.type]->getCallback(this);
	}
}

void Checkbox::drawObject(Window*) {
	Button::drawObject(nullptr);
	//
	text.setColor(oglWrapper::WHITE);
	text.printText(x + w + 5, y + h / 2 + 5);
}
