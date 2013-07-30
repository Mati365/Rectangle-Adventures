/*
 * Menu.cpp
 *
 *  Created on: 12 lip 2013
 *      Author: mateusz
 */
#include "Screens.hpp"

#include "../../Resources/Data/ResourceManager.hpp"

using namespace GameScreen;

Menu::Menu() :
		Game("menu.txt"),
		ver(oglWrapper::GRAY,
		    "Wersja: 0.5 beta | Autor: Mateusz Baginski | email:cziken58@gmail.com",
		    GLUT_BITMAP_HELVETICA_12,
		    12) {
	lvl->enableHUD(false);
	hero->getStatus()->health = 1;
	//
	createMenuEntries();
}

/**
 * Tworzenie komponentów menu!
 */
void Menu::createMenuEntries() {
	// Dodawanie pojedynczych przycisków do listy obiektów!
	const char* _entries[] = {
	                           "Kontynuuj",
	                           "Nowa gra",
	                           "Koniec gry" };
	for (usint i = 0; i < 3; ++i) {
		entries.push_back(
		        new Button(Rect<float>(210 + 135 * i, 590, 100, 40),
		                   _entries[i]));
	}

	// Dodawanie listy obiektów do świata!
	for (auto iter = entries.begin(); iter != entries.end(); ++iter) {
		(*iter)->putCallback(Event::MOUSE_RELEASED,
		                     dynamic_cast<Callback*>(this));
		lvl->getPhysics()->insert(*iter);
	}
}

/**
 * Odbieranie callbacku z przycisków w menu!
 */
void Menu::getCallback(Control* const & control) {
	for (usint i = 0; i < entries.size(); ++i) {
		/**
		 * Obsługa menu!
		 */
		if (entries[i] == control) {
			switch (i) {
				case 0:
					/**
					 * Kontynuacja gry
					 */
					break;

				case 1:
					/**
					 * Nowa gra
					 */
					splash->pushTitle(
					        "Tip: Skaczac na kupy pasek zycia regeneruj sie..",
					        211); // dla picu ;0
					active_screen = splash;
					splash->endTo(game);
					break;

				case 2:
					/**
					 * Koniec gry
					 */
					Engine::window_opened = false;
					break;
			}
			break;
		}
	}
	logEvent(Logger::LOG_INFO, "Otrzymano event w menu!");
}

/**
 * Odbieranie eventów z okna!
 */
void Menu::catchEvent(const Event& event) {
	for (auto iter = entries.begin(); iter != entries.end(); ++iter) {
		(*iter)->catchEvent(event);
	}
}

/**
 * Rysowanie menu
 */
void Menu::drawObject(Window* window) {
	if (lvl) {
		lvl->drawObject(window);
	}
	ver.printText(
	WINDOW_WIDTH - ver.getScreenLength() - 10,
	              22);
}

/**
 * Brak przecieku! 'lvl' usuwany w ~Game!!
 */
Menu::~Menu() {
	for (usint i = 0; i < entries.size(); ++i) {
		delete entries[i];
	}
	//
	logEvent(Logger::LOG_INFO, "Usuwanie obiektów menu zakończone sukcesem!");
}
