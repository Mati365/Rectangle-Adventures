/*
 * Menu.cpp
 *
 *  Created on: 12 lip 2013
 *      Author: mateusz
 */
#include "Screens.hpp"

#include "../../Tools/Tools.hpp"

#include "../../Resources/Data/ResourceManager.hpp"
#include "../../Resources/Data/SoundManager.hpp"

#include "../LevelManager.hpp"

using namespace GameScreen;

Menu::Menu() :
				Game("menu.txt"),
				ver(
						oglWrapper::GRAY,
						"Wersja: 0.6 | Autor: Mateusz Baginski | email:cziken58@gmail.com",
						GLUT_BITMAP_HELVETICA_12,
						12) {
	choose_sound = SoundManager::getInstance().getResourceSound(
			SoundManager::MENU_CHOOSE_SOUND);

	lvl->enableHUD(false);
	
	Character* hero = getHero();
	hero->getStatus()->health = 1;
	//
	createMenuEntries();
}

/** Tworzenie komponentow */
void Menu::createMenuEntries() {
	/** Dodawanie pojedynczych przyciskow do listy obiektow */
	const char* _entries[] = { "Kontynuuj", "Nowa gra", "Koniec gry" };
	for (usint i = 0; i < 3; ++i) {
		entries.push_back(
				new Button(
						Rect<float>(210 + 135 * i, 350, 100, 40),
						_entries[i]));
	}
	
	/** Dodawanie listy obiektow do swiata */
	for (auto iter = entries.begin(); iter != entries.end(); ++iter) {
		(*iter)->putCallback(
				Event::MOUSE_RELEASED,
				dynamic_cast<Callback*>(this));
		lvl->getPhysics()->insert(*iter);
	}
}

/** Odbieranie callbacku z przyciskow */
void Menu::getCallback(Control* const & control) {
	choose_sound->Play();

	for (usint i = 0; i < entries.size(); ++i) {
		/**
		 * Obsługa menu!
		 */
		if (entries[i] == control) {
			//
			switch (i) {
				case 0:
					/**
					 * Kontynuacja gry
					 */
					importSave();

					game = new Game();
					game->getMapRenderer()->addToParalax(
							loadMap("parallax_1.txt", MapINFO::WITHOUT_HERO),
							0.45f,
							game->getMapRenderer()->getHero(),
							ParalaxRenderer::PARALLAX
									| ParalaxRenderer::ROTATION
									| ParalaxRenderer::DRAW_QUAD);

					LevelManager::getInstance().setActualMap(
							SaveManager::getInstance().getSave()->stats[Save::LAST_LEVEL_INDEX]);
					/**
					 * Nowa gra
					 */
					active_screen = splash;
					splash->endTo(game);

					SDL_Delay(200);
					break;
					
				case 1: {
					safe_delete<Game>(game);
					game = new Game();

					game->getMapRenderer()->addToParalax(
							loadMap("parallax_1.txt", MapINFO::WITHOUT_HERO),
							0.45f,
							game->getMapRenderer()->getHero(),
							ParalaxRenderer::PARALLAX
									| ParalaxRenderer::ROTATION
									| ParalaxRenderer::DRAW_QUAD);
					/**
					 * Nowa gra
					 */
					active_screen = splash;
					splash->endTo(game);

					SDL_Delay(200);
				}
					break;
					
				case 2:
					/**
					 * Koniec gry
					 */
					window_config.putConfig(WindowConfig::WINDOW_OPENED, false);
					break;
			}
			break;
		}
	}
	logEvent(Logger::LOG_INFO, "Otrzymano event w menu!");
}

/** Event z okna */
void Menu::catchEvent(const Event& event) {
	Rect<float>& rect = *Camera::getFor().getPos();
	mouse.pos += Vector<int>(rect.x, rect.y);
	
	for (auto iter = entries.begin(); iter != entries.end(); ++iter) {
		(*iter)->catchEvent(event);
	}
	
	mouse.pos -= Vector<int>(rect.x, rect.y);
}

/** Rysowanie */
void Menu::drawObject(Window* window) {
	if (lvl) {
		lvl->drawObject(window);
	}
	ver.printText(screen_bounds.x / 2 - ver.getScreenLength() / 2, 20);
}

/** Brak przecieku! 'lvl' usuwany w ~Game!! */
Menu::~Menu() {
	safe_delete<sf::Sound>(choose_sound);
	//
	logEvent(Logger::LOG_INFO, "Usuwanie obiektów menu zakończone sukcesem!");
}
