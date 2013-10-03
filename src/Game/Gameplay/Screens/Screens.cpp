/*
 * Screens.cpp
 *
 *  Created on: 13 lip 2013
 *      Author: mateusz
 */
#include  "Screens.hpp"

#include "../../Resources/Data/SoundManager.hpp"

using namespace GameScreen;

/** Aktywny ekran w grze */
Panel* GameScreen::active_screen;

/** Inicjacja po zaladowaniu filesystem */
Game* GameScreen::game = nullptr;
Menu* GameScreen::menu = nullptr;
Splash* GameScreen::splash = nullptr;
Configuration* GameScreen::config = nullptr;
Ending* GameScreen::ending = nullptr;

/** Dzwiek w tle */
sf::Sound* GameScreen::background_sound = nullptr;

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
	splash->pushTitle("Mati365 presents..", 300, nullptr);
	splash->pushTitle("Rect Adventures", 300,
			readShape("iluzja_trojkat.txt", "iluzja_trojkat.txt", 0.f));
	active_screen = splash;

	/** Wczytywanie dzwieku */
	background_sound = SoundManager::getInstance().getResourceSound(
			SoundManager::BACKGROUND_SOUND_1, true);
	background_sound->Play();
}

/** Koniec gry */
void GameScreen::openEnding() {
	active_screen = ending;

	/** Zmiana dzwieki */
	if (background_sound) {
		background_sound->Stop();
		safe_delete<sf::Sound>(background_sound);
	}

	background_sound = SoundManager::getInstance().getResourceSound(
			SoundManager::BACKGROUND_SOUND_2, true);
	background_sound->Play();
}

/** Kasowanie! */
void GameScreen::unloadScreens() {
	safe_delete<Game>(game);
	safe_delete<Menu>(menu);
	safe_delete<Splash>(splash);
	safe_delete<Configuration>(config);
	safe_delete<Ending>(ending);

	/** Wywalanie dzwieku */
	background_sound->Stop();
	safe_delete<sf::Sound>(background_sound);
}
