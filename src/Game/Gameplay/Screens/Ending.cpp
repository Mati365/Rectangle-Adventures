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
								"++Rect Adventures++",
								"",
								"Udzwiekowanie:",
								"Mati365",
								"",
								"Choreografia:",
								"Mati365",
								"",
								"Kostiumy:",
								"Mati365",
								"",
								"Programowanie:",
								"Mati365",
								"",
								"Koszenie trawy:",
								"Mati365",
								"",
								"Scenariusz:",
								"Mati365",
								"",
								"Oswietlenie:",
								"Mati365",
								"",
								"Grafika:",
								"Mati365",
								"",
								"Skryptowanie:",
								"Mati365",
								"",
								"Tworzenie poziomow:",
								"Mati365",
								"",
								"----------------",
								"Specjalnie na konkurs interaktywny 2013!" };

Ending::Ending() :
				/** Tooltipy endingu */
				credit_tooltip(
						oglWrapper::WHITE,
						"",
						GLUT_BITMAP_HELVETICA_18,
						18),
				position(0),

				/** Wyjscie */
				exit(Rect<float>(0, 0, 100, 40), "Wyjdz"),

				/** Ilosc punktow */
				total_score(
						oglWrapper::ORANGE,
						"",
						GLUT_BITMAP_HELVETICA_18,
						18) {
	create();
}

/** Tworzenie */
void Ending::create() {
	exit.putCallback(Event::MOUSE_RELEASED, this);

	addControl(&exit);
}

/** Callback od przyciskow! */
void Ending::getCallback(Control* const & obj) {
	window_config.putConfig(WindowConfig::WINDOW_OPENED, false);
}

/** Rendering */
void Ending::drawObject(Window*) {
	/** Czy napis powinien sie zatrzymac? */
	float begin_position = screen_bounds.y - position;
	if (begin_position > -16 * 24) {
		position += 2.f;
	}

	/** Rendering */
	for (usint i = 0; i < 34; ++i) {
		glText* credit = &credit_tooltip;

		credit->setString(credits[i], -1);
		credit->printText(
				screen_bounds.x / 2 - credit->getScreenLength() / 2,
				screen_bounds.y + (i + 1) * 24 - position);
	}

	total_score.setString(
			"Zdobyles: "
					+ Convert::toString<usint>(
							game->getHero()->getStatus()->score * 10)
					+ "pkt :)",
			-1);

	total_score.printText(
			screen_bounds.x / 2 - total_score.getScreenLength() / 2,
			screen_bounds.y + 34 * 25 - position);

	/** Wyjscie */
	exit.y = screen_bounds.y - exit.h - 5;
	exit.x = screen_bounds.x - 120;

	exit.drawObject(nullptr);
}

