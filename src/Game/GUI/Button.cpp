/*
 * Button.cpp
 *
 *  Created on: 25-06-2013
 *      Author: mateusz
 */
#include "GUI.hpp"

using namespace GUI;

Button::Button(const Rect<float>& _bounds, const char* _text,
		bool _border_enabled, Callback* _callback) :
				Control(_bounds),
				text(oglWrapper::WHITE, _text, GLUT_BITMAP_HELVETICA_18, 18) {
	border_enabled = _border_enabled;
	if (_callback) {
		putCallback(Event::MOUSE_RELEASED, _callback);
	}
}

void Button::drawObject(Window*) {
	/**
	 * Nie wymaga optymalizacji!
	 */
	switch (control_state) {
		case NORMAL:
			if (border_enabled) {
				oglWrapper::beginStroke(0xAAAA);
				oglWrapper::drawRect(x, y, w, h, oglWrapper::WHITE, 2);
				oglWrapper::endStroke();
			}
			if (old_state != control_state) {
				text.setColor(oglWrapper::WHITE);
			}
			break;
			/**
			 *
			 */
		case CLICKED:
			oglWrapper::drawFillRect(x, y, w, h, oglWrapper::WHITE);
			if (old_state != control_state) {
				text.setColor(oglWrapper::BLACK);
			}
			break;
	}
	text.printText(
			x + w / 2 - text.getScreenLength() / 2,
			y + h / 2 + text.getFontHeight() / 2 - 4);
	//
	old_state = control_state;
}

