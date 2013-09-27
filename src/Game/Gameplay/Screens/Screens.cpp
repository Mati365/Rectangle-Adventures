/*
 * Screens.cpp
 *
 *  Created on: 13 lip 2013
 *      Author: mateusz
 */
#include  "Screens.hpp"

using namespace GameScreen;

/** Aktywny ekran w grze */
Screen* GameScreen::active_screen;

/** Inicjacja po zaladowaniu filesystem */
Game* GameScreen::game = nullptr;
Menu* GameScreen::menu = nullptr;
Splash* GameScreen::splash = nullptr;
Configuration* GameScreen::config = nullptr;
Ending* GameScreen::ending = nullptr;

/** Wczytywanie configu */
void GameScreen::openConfig() {
	config = new Configuration();
	ending = new Ending();
	
	active_screen = config;
}

/** Wczytywanie */
void GameScreen::loadScreens() {
	menu = new Menu();
	
	/** Konfiguracja splasha */
	splash = new Splash();
	splash->endTo(menu);
	/**
	splash->pushTitle("Mati365 presents..", 400, nullptr);
	splash->pushTitle(
			"Rect Adventures",
			400,
			readShape("iluzja_trojkat.txt", "iluzja_trojkat.txt", 33.f));
	*/
	active_screen = menu;
}

/** Koniec gry */
void GameScreen::openEnding() {
	active_screen = ending;
}

/** Kasowanie! */
void GameScreen::unloadScreens() {
	safe_delete<Game>(game);
	safe_delete<Menu>(menu);
	safe_delete<Splash>(splash);
	safe_delete<Configuration>(config);
	safe_delete<Ending>(ending);
}
