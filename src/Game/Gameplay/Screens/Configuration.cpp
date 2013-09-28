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
													"640 x 480 32bit :>",
													"800 x 600 32bit :)",
													"1024 x 768 32bit :|",
													"Natywna ;(" };

/** Obssugiwane sterowania */
string Configuration::supported_controls[] = { "WSAD", "Strzalki" };

Configuration::Configuration() :
				/** Lista rozdzialek */
				resolution_list(
						Rect<float>(
								screen_bounds.x / 2 - 205,
								screen_bounds.y / 2 - 50,
								200,
								100),
						this),
				res_list_tooltip(
						oglWrapper::WHITE,
						"Rozdzielczosc ekranu:",
						GLUT_BITMAP_HELVETICA_18,
						18),

				/** Lista kontrolek */
				controls_list(
						Rect<float>(
								screen_bounds.x / 2 + 5,
								screen_bounds.y / 2 - 5,
								200,
								55),
						this),
				controls_list_tooltip(
						oglWrapper::WHITE,
						"Sterowanie:",
						GLUT_BITMAP_HELVETICA_18,
						18),

				/** Zatwierdzenie */
				fullscreen(
						Rect<float>(
								screen_bounds.x / 2 - 200,
								screen_bounds.y / 2 + 140),
						"Caly ekran"),
				shaders(
						Rect<float>(
								screen_bounds.x / 2 - 200,
								screen_bounds.y / 2 + 80),
						"Ze shaderami(mozliwy crash przy GPU Intel GMA)"),
				enter(
						Rect<float>(
								screen_bounds.x / 2 - 50,
								screen_bounds.y - 150,
								100,
								40),
						"Graj!") {
	create();
}

/** Tworzenie */
void Configuration::create() {
	for (usint i = 0; i < 4; ++i) {
		resolution_list.addListItem(supported_resolutions[i]);
		if (i < 2) {
			controls_list.addListItem(supported_controls[i]);
		}
	}
	enter.putCallback(Event::MOUSE_CLICKED, this);

	/** Domyslne wartosci */
	fullscreen.setSelected(true);
	shaders.setSelected(true);

	/** Dodawanie */
	addControl(&fullscreen);
	addControl(&shaders);
	addControl(&enter);
	addControl(&controls_list);
	addControl(&resolution_list);
}

/** Callback od przyciskow! */
void Configuration::getCallback(Control* const & obj) {
	string res_selected = resolution_list.getSelectedItem();
	
	/** Zmiana rozdzielczosci */
	if (res_selected == "Natywna") {
		screen_bounds.x = screen_bounds.y = 0;
	} else {
		sscanf(
				res_selected.c_str(),
				"%f x %f 32bit",
				&screen_bounds.x,
				&screen_bounds.y);
	}
	window_config.putConfig(WindowConfig::RESOLUTION_CHANGED, true);

	/** Zmiana sterowania */
	window_config.putConfig(
			WindowConfig::WSAD_CONTROLS,
			controls_list.getSelectedItem() == "WSAD");

	/** Shadery */
	window_config.putConfig(WindowConfig::WITH_SHADERS, shaders.isChecked());

	/** Fullscreen */
	window_config.putConfig(WindowConfig::FULLSCREEN, fullscreen.isChecked());
}

/** Rysowanie z tooltipem */
void Configuration::drawWithTooltip(SelectList* list, glText* tooltip) {
	tooltip->printText(
			list->x + list->w / 2 - tooltip->getScreenLength() / 2,
			list->y - 20);
	list->drawObject(nullptr);
}

/** Rendering */
void Configuration::drawObject(Window*) {
	drawWithTooltip(&resolution_list, &res_list_tooltip);
	drawWithTooltip(&controls_list, &controls_list_tooltip);

	Panel::drawObject(nullptr);
}
