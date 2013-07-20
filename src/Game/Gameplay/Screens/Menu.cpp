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
		ver(oglWrapper::WHITE,
			"Wersja: 0.5 alpha | Autor: Mateusz Baginski | email:cziken58@gmail.com",
			GLUT_BITMAP_HELVETICA_12,
			12) {
	lvl->enableHUD(false);
	//
	createMenuEntries();
}

/**
 * Tworzenie komponentów menu!
 */
void Menu::createMenuEntries() {
	// Dodawanie pojedynczych przycisków do listy obiektów!
	entries.push_back(
			new Button(Rect<float>(110, 200, 100, 40), "Menu akcji:", false));
	entries.push_back(new Button(Rect<float>(110, 250, 100, 40), "Kontynuuj"));
	entries.push_back(new Button(Rect<float>(110, 300, 100, 40), "Nowa gra"));
	entries.push_back(new Button(Rect<float>(110, 350, 100, 40), "Koniec gry"));

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
				case 1:
					/**
					 * Kontynuacja gry
					 */
					break;

				case 2:
					/**
					 * Nowa gra
					 */
					active_screen = game;
					break;

				case 3:
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
					WINDOW_HEIGHT - ver.getFontHeight() - 2);
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
