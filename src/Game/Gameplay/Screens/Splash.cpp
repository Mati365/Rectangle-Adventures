/*
 * Splash.cpp
 *
 *  Created on: 15 lip 2013
 *      Author: mateusz
 */
#include "Screens.hpp"

using namespace GameScreen;

Splash::Splash() :
		title(oglWrapper::WHITE, "",
		GLUT_BITMAP_HELVETICA_18,
				18) {
}

void Splash::catchEvent(const Event&) {

}

void Splash::drawObject(Window*) {
	if (strlen(title.getString()->c_str()) == 0 && !texts.empty()) {
		title.setString(texts.back()->text, -1);
	}
	if (texts.empty()) {
		return;
	}
	SplashInfo* _text = texts.back();
	_text->timer++;
	if (_text->timer > _text->visible_time) {
		if (texts.size() == 1) {
			active_screen = menu;
		} else {
			delete _text;
			texts.pop_back();
		}
		//
		title.setString(texts.back()->text, -1);
	}
	title.getColor()->a = 255.f
			* (1.f - (float) _text->timer / (float) _text->visible_time);
	title.printText(
			WINDOW_WIDTH / 2 - title.getScreenLength() / 2,
			WINDOW_HEIGHT / 2 - title.getLinesOfText() * title.getFontHeight());
}

Splash::~Splash() {
	for (usint i = 0; i < texts.size(); ++i) {
		delete texts[i];
	}
}
