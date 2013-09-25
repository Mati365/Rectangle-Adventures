/*
 * SelectList.cpp
 *
 *  Created on: 24-09-2013
 *      Author: bagin_000
 */
#include "GUI.hpp"

using namespace GUI;

SelectList::SelectList(const Rect<float>& _bounds, Callback* _callback,
		string* _elements) :
				Control(_bounds) {
	if (_callback) {
		putCallback(Event::MOUSE_RELEASED, _callback);
	}
}

/** Event z okna */
void SelectList::catchEvent(const Event& event) {
	if (!border_enabled || !isMouseCollision(x, y, w, h)) {
		control_state = NORMAL;
		return;
	}
	/**
	 * _elements.size = h
	 * id_zaznaczonego = mouse.y
	 * ---------------------
	 * id_zaznaczonego = _elements.size * mouse.y / h
	 */
	usint id = elements.size() * (mouse.pos.y - this->y) / h;
	if (id < elements.size()) {
		selected_item = *elements[id].getString();
	}
}

/** Rysowanie */
void SelectList::drawObject(Window*) {
	if (selected_item.empty()) {
		selected_item = *elements[0].getString();
	}
	
	/** Rendering obramowania */
	if (border_enabled) {
		oglWrapper::beginStroke(0xAAAA);
		oglWrapper::drawRect(x, y, w, h, oglWrapper::WHITE, 2);
		oglWrapper::endStroke();
	}
	
	/** Rendering element�w */
	for (usint i = 0; i < elements.size(); ++i) {
		bool selected = selected_item == *elements[i].getString();
		
		if (selected) {
			elements[i].setColor(oglWrapper::BLACK);
			oglWrapper::drawFillRect(
					x + 5,
					y + i * 22 + 6,
					w - 10,
					22,
					oglWrapper::WHITE);
			
		} else {
			elements[i].setColor(oglWrapper::WHITE);
		}
		
		elements[i].printText(x + 5, y + (i + 1) * 22);
		if (i != elements.size() - 1) {
			float y = this->y + (i + 1) * 22 + 6;
			
			oglWrapper::beginStroke(0xA0A0);
			oglWrapper::drawLine(x + 5, y, x + w - 10, y, oglWrapper::WHITE, 1);
			oglWrapper::endStroke();
		}
	}
	
	//
	old_state = control_state;
}
