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
	 * Wczytywanie ekranow musi byv
	 * PO wczytaniu systemu plikow!
	 */
	void openConfig();
	void openEnding();
	
	void loadScreens();
	void unloadScreens();
	
	/** Ekran gry */
	class Game: public Screen {
		protected:
			MapRenderer* lvl;

		public:
			Game(const char* = nullptr);

			virtual void catchEvent(const Event&);
			virtual void drawObject(Window*);

			Character* getHero() const {
				return lvl->getHero();
			}
			
			MapRenderer* getMapRenderer() const {
				return lvl;
			}
			
			~Game() {
				safe_delete<MapRenderer>(lvl);
				logEvent(
						Logger::LOG_INFO,
						"Usuwanie obiektów sceny zakończone sukcesem!");
			}
	};
	
	/** Menu gry! */
	class Menu: public Game, public Callback {
		private:
			/** Podstawowe info o grze */
			glText ver;

			/** Elementy menu */
			deque<Control*> entries;

		public:
			Menu();

			virtual void drawObject(Window*);

			/** Event z okna */
			virtual void catchEvent(const Event&);

			MapRenderer* getMapRenderer() {
				return lvl;
			}
			
			/** Callback od przyciskow! */
			void getCallback(Control* const &);

			~Menu();

		private:
			void createMenuEntries();
	};
	
	/** Splash - czarny ekran z tekstem */
	class Splash: public Screen {
		public:
			class SplashInfo {
				public:
#define LOGO_WIDTH 228
					/** Tytul */
					char* text;
					_Timer timer;

					/** Logo podczas pierwszego wlaczenia */
					IrregularPlatform* logo;

					SplashInfo(const char*, usint, PlatformShape* = NULL);

					~SplashInfo();
			};

		protected:
			deque<SplashInfo*> texts;
			glText title;

			/** Powrot po splashu! */
			Screen* return_to;

		public:
			Splash();

			virtual void drawObject(Window*);

			/** Czyszczenie! */
			void unload() {
				for (usint i = 0; i < texts.size(); ++i) {
					delete texts[i];
				}
				texts.clear();
			}
			
			/** Przerzucanie po splashu! */
			void endTo(Screen* _return_to) {
				return_to = _return_to;
			}
			
			/** Dodawanie tytulu do splasha */
			void pushTitle(const char* _title, usint _visible_time,
					PlatformShape* _logo = NULL) {
				texts.push_front(new SplashInfo(_title, _visible_time, _logo));
			}
			
			~Splash();

		private:
			/** Powrot do starego ekranu */
			void returnScreen();
	};
	
	/** Ekran konfiguracji ustawien grafiki */
	class Configuration: public Screen, public Callback {
		private:
			static string supported_resolutions[];
			static string supported_controls[];

			/** Lista rozdzielczosci */
			SelectList resolution_list;

			/** Tekst nad rozdizelczoscami */
			glText res_list_tooltip;

			/** Lista dostepnych sterowan */
			SelectList controls_list;

			/** Tekst nad rozdizelczoscami */
			glText controls_list_tooltip;

			/** Zatwierdzenie konfiguracji */
			Button enter;

		public:
			Configuration();

			virtual void drawObject(Window*);

			/** Event z okna */
			virtual void catchEvent(const Event&);

			/** Callback od przyciskow! */
			void getCallback(Control* const &);

		private:
			/** Rysowanie z tooltipem */
			void drawWithTooltip(SelectList*, glText*);
	};

	/** Zakonczenie gry */
	class Ending: public Screen, public Callback {
		private:
			/** Napisy koncowe */
			static string credits[];

			/** Pojedynczy napis */
			glText credit_tooltip;
			glText author_tooltip;

			/** Pozycja w unoszeniu się w górę */
			int position;

			/** Wyjscie */
			Button exit;

		public:
			Ending();

			virtual void drawObject(Window*);

			/** Event z okna */
			virtual void catchEvent(const Event&);

			/** Callback od przyciskow! */
			void getCallback(Control* const &);

	};

	/** Aktywne ekrany  */
	extern Game* game; // okno gameplay
	extern Menu* menu; // menu gry
	extern Splash* splash; // splash
	extern Configuration* config; // Konfiguracja grafy
	extern Ending* ending; // Zakonczenie

	extern Screen* active_screen;
}

#endif /* SCREENS_HPP_ */
