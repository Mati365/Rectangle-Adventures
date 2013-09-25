/*
 * Panel.cpp
 *
 *  Created on: 23-06-2013
 *      Author: mateusz
 */
#include "GUI.hpp"

using namespace GUI;

/** For szybsze niz iteratory jest */
void Panel::drawObject(Window*) {
	for (usint i = 0; i < objects.size(); ++i) {
		objects[i]->drawObject(NULL);
	}
}

void Panel::catchEvent(const Event& event) {
	for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
		(*iter)->catchEvent(event);
	}
}
