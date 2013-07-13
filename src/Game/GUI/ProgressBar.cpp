/*
 * ProgressBar.cpp
 *
 *  Created on: 13-05-2013
 *      Author: mateusz
 */
#include "GUI.hpp"

using namespace GUI;

ProgressBar::ProgressBar(const Rect<float>& _bounds, const Color& _col,
		usint _max_value, usint _position) :
		Control(_bounds),
		max_value(_max_value),
		value(0) {
	col = _col;
	position = _position;
}

void ProgressBar::drawObject(Window*) {
	/**
	 * Obramowanie
	 */
	if (border_enabled) {
		oglWrapper::drawRect(x, y, w, h, oglWrapper::WHITE, 1);
	}
	/**
	 * Wypełnienie:
	 * + Horyzontalne
	 * + Vertykalne
	 */
	float size = (float) value / (float) max_value * (position == HORIZONTAL ?
			h : w - 5);
	if (value > max_value) {
		size = max_value;
	}
	switch (position) {
		case HORIZONTAL:
			oglWrapper::drawFillRect(x + 3, y + h - 3 - size, w - 5, size, col);
			oglWrapper::drawFillRect(x + w - 2 - w * 0.1, y + h - 3 - size,
					w * 0.1, size, Color(0, 0, 0, 100));
			break;
			/**
			 *
			 */
		case VERTICAL:
			oglWrapper::drawFillRect(x + 3, y + 2, size, h - 6, col);
			oglWrapper::drawFillRect(x + 3, y + h - 4 - h * 0.1, size, h * 0.1,
					Color(0, 0, 0, 100));
			break;
	}
}
