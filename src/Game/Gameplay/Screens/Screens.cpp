/*
 * Screens.cpp
 *
 *  Created on: 13 lip 2013
 *      Author: mateusz
 */
#include  "Screens.hpp"

using namespace GameScreen;

// Aktywny ekran w grze
Screen* GameScreen::active_screen;

/**
 * Inicjacja dopiero po załadowaniu filesystem'u!
 */
Game* GameScreen::game = NULL;
Menu* GameScreen::menu = NULL;
Splash* GameScreen::splash = NULL;

/**
 * Wczytywanie
 */
void GameScreen::loadScreens() {
	game = new Game("mapa.txt");
	//menu = new Menu();
	splash = new Splash();
}

/**
 * Kasowanie!
 */
void GameScreen::unloadScreens() {
	if (game) {
		delete game;
	}
	if (menu) {
		delete menu;
	}
	if (splash) {
		delete splash;
	}
}
