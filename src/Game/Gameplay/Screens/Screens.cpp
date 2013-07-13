/*
 * Screens.cpp
 *
 *  Created on: 13 lip 2013
 *      Author: mateusz
 */
#include  "Screens.hpp"

using namespace GameScreen;

// Aktywny ekran w grze
Game* GameScreen::active_screen;

/**
 * Inicjacja dopiero po załadowaniu filesystem'u!
 */
Game* GameScreen::game = NULL;
Menu* GameScreen::menu = NULL;

/**
 * Wczytywanie
 */
void GameScreen::loadScreens() {
	game = new Game("mapa.txt");
	menu = new Menu();
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
}
