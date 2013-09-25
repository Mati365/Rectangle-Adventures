/*
 * Configuration.cpp
 *
 *  Created on: 24-09-2013
 *      Author: bagin_000
 */
#include "Screens.hpp"

using namespace GameScreen;

/** Obssugiwane rozdzialki */
string Configuration::supported_resolutions[] = {
													"640 x 480 32bit",
													"800 x 600 32bit",
													"1024 x 768 32bit",
													"Natywna" };

Configuration::Configuration() :
				resolution_list(
						Rect<float>(
								screen_bounds.x / 2 - 100,
								screen_bounds.y / 2 - 50,
								200,
								100),
						this),
				enter(
						Rect<float>(
								screen_bounds.x / 2 - 50,
								screen_bounds.y - 150,
								100,
								40),
						"Graj!"),
				res_list_tooltip(
						oglWrapper::WHITE,
						"Rozdzielczosc ekranu:",
						GLUT_BITMAP_HELVETICA_18,
						18) {
	
	for (usint i = 0; i < 4; ++i) {
		resolution_list.addListItem(supported_resolutions[i]);
	}
	enter.putCallback(Event::MOUSE_RELEASED, this);
}

/** Callback od przyciskow! */
void Configuration::getCallback(Control* const & obj) {
	string selected = resolution_list.getSelectedItem();
	
	if (selected == "Natywna") {
		screen_bounds.x = screen_bounds.y = 0;
	} else {
		sscanf(
				selected.c_str(),
				"%f x %f 32bit",
				&screen_bounds.x,
				&screen_bounds.y);
	}
	resolution_changed = true;
}

/** Event z okna */
void Configuration::catchEvent(const Event& event) {
	resolution_list.catchEvent(event);
	enter.catchEvent(event);
}

void Configuration::drawObject(Window*) {
	res_list_tooltip.printText(
			screen_bounds.x / 2 - res_list_tooltip.getScreenLength() / 2,
			resolution_list.y - 40);
	resolution_list.drawObject(nullptr);

	enter.drawObject(nullptr);
}
