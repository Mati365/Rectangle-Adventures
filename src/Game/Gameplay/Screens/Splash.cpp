/*
 * Splash.cpp
 *
 *  Created on: 15 lip 2013
 *      Author: mateusz
 */
#include "Screens.hpp"

using namespace GameScreen;

/**
 * Informacje o splashu
 */
Splash::SplashInfo::SplashInfo(const char* _text, usint _visible_time,
		PlatformShape* _logo) :
				text(Convert::getDynamicValue(_text)),
				timer(_visible_time),
				logo(NULL) {
	if (_logo) {
		logo = new IrregularPlatform(
				WINDOW_WIDTH / 2 - LOGO_WIDTH / 2,
				0,
				0,
				_logo);
		logo->fitToWidth(LOGO_WIDTH);
		logo->y = WINDOW_HEIGHT / 2 - logo->h / 2 - 40;
	}
}

Splash::SplashInfo::~SplashInfo() {
	if (text) {
		delete[] text;
	}
	if (logo) {
		delete logo;
	}
}

/**
 * Ekran splash na czarnym tle!
 */
Splash::Splash() :
				title(oglWrapper::WHITE, "", GLUT_BITMAP_HELVETICA_18, 18),
				return_to(NULL) {
}

void Splash::drawObject(Window*) {
	SplashInfo* _text = texts.back();

	//
	if (strlen(title.getString()->c_str()) == 0 && !texts.empty()) {
		title.setString(_text->text, -1);
	}
	_text->timer.tick();

	// Tekst
	title.getColor()->a = (usint) (255.f
			* (1.f
					- (float) _text->timer.cycles_count
							/ (float) _text->timer.max_cycles_count));
	title.printText(
			WINDOW_WIDTH / 2 - title.getScreenLength() / 2,
			!_text->logo ?
					WINDOW_HEIGHT / 2
							- title.getLinesOfText() * title.getFontHeight() :
					_text->logo->y + _text->logo->h + 50);

	// Logo
	if (_text->logo) {
		_text->logo->drawObject(NULL);
		//
		PlatformShape* shape = _text->logo->getShape();
		shape->setLineWidth(3);

		// Zanikanie
		shape->getMainColor()->r = title.getColor()->a;
		shape->getMainColor()->g = title.getColor()->a;
		shape->getMainColor()->b = title.getColor()->a;
		shape->recompile();
	}

	/**
	 * Timer, efekt zanikania
	 */
	if (!_text->timer.active) {
		delete _text;
		texts.pop_back();
		//
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
}

Splash::~Splash() {
	unload();
}
