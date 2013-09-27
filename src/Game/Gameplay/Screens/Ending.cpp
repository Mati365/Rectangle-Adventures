/*
 * Ending.cpp
 *
 *  Created on: 27-09-2013
 *      Author: mateusz
 */
/*
 * Ending.cpp
 *
 *  Created on: 24-09-2013
 *      Author: bagin_000
 */
#include "Screens.hpp"

using namespace GameScreen;

/** Napisy koncowe */
string Ending::credits[] = {
								"Rect Adventures",
								"Udzwiekowanie:",
								"Mati365",
								"Choreografia:",
								"Mati365",
								"Kostiumy:",
								"Mati365",
								"Programowanie:",
								"Mati365",
								"Koszenie trawy:",
								"Mati365",
								"Specjalnie na konkurs interaktywny 2013!" };

Ending::Ending() :
				/** Tooltipy endingu */
				credit_tooltip(
						oglWrapper::WHITE,
						"",
						GLUT_BITMAP_HELVETICA_18,
						18),
				author_tooltip(
						oglWrapper::WHITE,
						"",
						GLUT_BITMAP_HELVETICA_12,
						12),
				position(0),

				/** Wyjscie */
				exit(Rect<float>(0, 0, 100, 40), "Wyjdz") {
	exit.putCallback(Event::MOUSE_RELEASED, this);
}

/** Callback od przyciskow! */
void Ending::getCallback(Control* const & obj) {
	window_config.putConfig(WindowConfig::WINDOW_OPENED, false);
}

/** Event z okna */
void Ending::catchEvent(const Event& event) {
	exit.catchEvent(event);
}

/** Rendering */
void Ending::drawObject(Window*) {
	/** Czy napis powinien sie zatrzymac? */
	float begin_position = screen_bounds.y - position;
	if (begin_position > 50) {
		position += 2.f;
	}

	/** Rendering */
	for (usint i = 0; i < 12; ++i) {
		glText* credit = (i % 2) ? &author_tooltip : &credit_tooltip;

		credit->setString(credits[i], -1);
		credit->printText(
				screen_bounds.x / 2 - credit->getScreenLength() / 2,
				screen_bounds.y + (i + 1) * 24 - position);
	}

	/** Wyjscie */
	exit.y = screen_bounds.y - exit.h - 5;
	exit.x = screen_bounds.x - 120;

	exit.drawObject(nullptr);
}

