/*
 * Screens.hpp
 *
 *  Created on: 23-06-2013
 *      Author: mateusz
 */

#ifndef SCREENS_HPP_
#define SCREENS_HPP_
#include "../../Engine/Graphics/Engine.hpp"

#include "../Gameplay.hpp"

using namespace Engine;
using namespace Gameplay;
using namespace GUI;

/**
 * Ekrany w grze, np: ekran menu, ekran gry, ekran edytora map
 */
using namespace oglWrapper;

namespace GameScreen {
	/**
	 * Ekran gry!
	 * Wzorzec Singleton!
	 */
	class Screen: public Renderer {
		public:
			virtual void drawObject(Window*)=0;
			virtual void catchEvent(const Event&) {
			}
			
			virtual ~Screen() {
			}
	};
	
	/**
	 * Aktywne ekrany
	 */
	class Game;
	class Menu;
	class Splash;
	
	extern Game* game; // okno gameplay
	extern Menu* menu; // menu gry
	extern Splash* splash; // splash
	
	extern Screen* active_screen;
	
	/**
	 * Wczytywanie ekranów musi być
	 * PO wczytaniu systemu plików!
	 */
	void loadScreens();
	void unloadScreens();
	
	/**
	 * Ekran gry
	 */
	class Game: public Screen {
		protected:
			MapRenderer* lvl;
			Character* hero;

		public:
			Game(const char*);

			virtual void catchEvent(const Event&);
			virtual void drawObject(Window*);

			Character* getHero() const {
				return hero;
			}
			
			MapRenderer* getMapRenderer() const {
				return lvl;
			}
			
			~Game() {
				if (lvl) {
					delete lvl;
				}
				logEvent(
						Logger::LOG_INFO,
						"Usuwanie obiektów sceny zakończone sukcesem!");
			}
	};
	/**
	 * Menu gry!
	 */
	class Menu: public Game, public Callback {
		private:
			// Podstawowe info o grze
			glText ver;
			// Elementy menu
			deque<Control*> entries;

		public:
			Menu();

			virtual void catchEvent(const Event&);
			virtual void drawObject(Window*);

			MapRenderer* getMapRenderer() {
				return lvl;
			}

			/**
			 * Callback od przycisków!
			 */
			void getCallback(Control* const &);

			~Menu();

		private:
			void createMenuEntries();
	};
	/**
	 * Splash - początkowy czarny ekran
	 * z tekstem o autorze.
	 */
	class Splash: public Screen {
		public:
			class SplashInfo {
				public:
#define LOGO_WIDTH 228
					/**
					 * Tytuł
					 */
					char* text;
					_Timer timer;

					/**
					 * Logo podczas pierwszego włącznia
					 */
					IrregularPlatform* logo;

					SplashInfo(const char*, usint, PlatformShape* = NULL);

					~SplashInfo();
			};

		protected:
			deque<SplashInfo*> texts;
			glText title;

			// Powrót po splashu!
			Screen* return_to;

		public:
			Splash();

			virtual void drawObject(Window*);

			/**
			 * Czyszczenie!
			 */
			void unload() {
				for (usint i = 0; i < texts.size(); ++i) {
					delete texts[i];
				}
				texts.clear();
			}
			
			/**
			 * Przerzucanie po splashu!
			 */
			void endTo(Screen* _return_to) {
				return_to = _return_to;
			}
			
			void pushTitle(const char* _title, usint _visible_time,
					PlatformShape* _logo = NULL) {
				texts.push_front(new SplashInfo(_title, _visible_time, _logo));
			}
			
			~Splash();
	};
}

#endif /* SCREENS_HPP_ */
