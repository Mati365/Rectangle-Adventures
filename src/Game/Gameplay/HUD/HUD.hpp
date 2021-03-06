/*
 * HUD.hpp
 *
 *  Created on: 29-05-2013
 *      Author: mateusz
 */

#ifndef HUD_HPP_
#define HUD_HPP_

#include "../Objects/Objects.hpp"
#include "../../GUI/GUI.hpp"

using namespace GUI;

#define SPACES 5 // Odstepy miedzy ramke a krawedzia� okna
/**
 * To�o intra np. mapa, ktora musi byc
 * zastopowana!
 */
class IntroBackground {
	public:
		enum State {
			PAUSE,
			RESUME,
			STOP
		};

	protected:
		usint state;

	public:
		IntroBackground() :
						state(RESUME) {
		}
		
		/** Stan intra */
		usint getState() const {
			return state;
		}
		void setState(usint _state) {
			state = _state;
		}
		
		virtual Character* getHero() = 0;

		virtual ~IntroBackground() {
		}
};

/**
 * Pomiedzy wiadomosciami mozliwe jest
 * wczytywanie cutscenek!
 */
class MessageRenderer: public Renderer, public EventListener, public Callback {
#define SCORE_ICON_WIDTH 14
#define HEART_ICON_WIDTH 16
		
	public:
		enum Screen {
			INTRO_SCREEN,
			DEATH_SCREEN,
			HUD_SCREEN
		};

		class Message {
			public:
				string title;
				string text;
				PlatformShape* cutscene;

				Message(const string& _title, const string& _text,
						const char* _cutscene_path = NULL) :
								title(_title),
								text(_text),
								cutscene(NULL) {
					if (_cutscene_path)
						cutscene = readShape(_cutscene_path, _cutscene_path, 0);
				}
		};
	private:
		float height;

		/** Typy powiadomien */
		usint screen;

		/** Kolejka wiadomosci */
		deque<Message> msgs;

		glText text;
		glText title;

		Color border_color;
		Color background_color;

		/**
		 * ########################################### HUD!
		 */

		/** temperatura */
		usint hud_temperature;

		IrregularPlatform heart;
		ProgressBar health_bar;

		/** animacja skurczu serca */
		_Timer heart_anim;

		IrregularPlatform score;
		ProgressBar score_bar;

		/** reload gry */
		Button retry_hud;

		/** Ekran smierci */
		glText game_over;
		Button* retry_game;
		Button* return_to_menu;

		/** Wiadomosci w intro od i do.. */
		IntroBackground* background;
		Character* hero;

		/** Miejsce na cutscene */
		IrregularPlatform* cutscene_box;

		/**
		 * ########################################### Informacje!
		 */

		/** Klawisze sterowania */
		map<usint, IrregularPlatform*> arrows_icons;

		/** Napis sterowania */
		glText controls_tooltip;

		/** Procent przjescia gry */
		glText game_progress_tooltip;
		glText game_progress;

		/** Ilosc punktow */
		glText game_score_tooltip;
		glText game_score;

	public:
		MessageRenderer(float, const Color&, const Color&, IntroBackground*);

		virtual void drawObject(Window*);
		virtual void catchEvent(const Event&);

		/** Callback od przyciskow! */
		void getCallback(Control* const &);

		bool popMessage();
		void addMessage(const Message&);

		/** Ekran smierci */
		void setScreen(usint _screen) {
			screen = _screen;
		}
		
		usint getActiveScreen() const {
			return screen;
		}
		
		/**
		 * Jesli zamkniete - pokazywane jest
		 * intro.
		 */
		usint getScreen() const {
			return screen;
		}
		
		bool hasMessages() const {
			return !msgs.empty();
		}
		
		float getHeight() const {
			return height;
		}
		
		~MessageRenderer() {
			safe_delete<IrregularPlatform>(cutscene_box);
			safe_delete<Button>(retry_game);
			safe_delete<Button>(return_to_menu);
		}
		
	private:
		/** Tworzenie */
		void create();

		/** Odswiezanie */
		void updateHUDControls();

		/** Otwieranie przerywnika pomiedzy scenami! */
		void openCutscene(const Message&);
		void closeCutscene();

		/** Rysowanie ekranow */
		void drawPlayerHUD(Window*);
		void drawIntroMessage(Window*);
		void drawDeathScreen(Window*);

		/** Informacje */
		void drawGameInfo(Window*);

		/** Rysowanie klawiszologii */
		void drawControls(Window*);

		/** Obramowanie */
		void drawBorder(Window*);
};

#endif /* HUD_HPP_ */
