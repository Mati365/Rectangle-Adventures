/*
 * Splash.cpp
 *
 *  Created on: 15 lip 2013
 *      Author: mateusz
 */
#include "Screens.hpp"

using namespace GameScreen;

Splash::Splash() :
				title(oglWrapper::WHITE, "", GLUT_BITMAP_HELVETICA_18, 18),
				return_to(NULL) {
}

void Splash::drawObject(Window*) {
	SplashInfo* _text = texts.back();
	
	if (strlen(title.getString()->c_str()) == 0 && !texts.empty()) {
		title.setString(_text->text, -1);
	}
	_text->timer++;
	if (_text->timer > _text->visible_time) {
		delete _text;
		texts.pop_back();
		if (texts.empty()) {
			title.setString("", 0);
			if (return_to) {
				active_screen = return_to;
			}
			return;
		}
		//
		title.setString(texts.back()->text, -1);
	}
	title.getColor()->a = (usint) (255.f
			* (1.f - (float) _text->timer / (float) _text->visible_time));
	title.printText(
			WINDOW_WIDTH / 2 - title.getScreenLength() / 2,
			WINDOW_HEIGHT / 2 - title.getLinesOfText() * title.getFontHeight());
}

Splash::~Splash() {
	unload();
}
