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

#define SPACES 5 // Odstępy między ramką a krawędzią okna
/**
 * Tło intra np. mapa, która musi być
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
		
		/**
		 * Stan intra
		 */
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
 * Pomiędzy wiadomościami możliwe jest
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

		/**
		 * Typy powiadomień
		 */
		usint screen;

		/**
		 * Kolejka wiadomości
		 */
		deque<Message> msgs;

		glText text;
		glText title;

		Color border_color;
		Color background_color;

		/**
		 * HUD!
		 */
		// temperatura
		usint hud_temperature;

		IrregularPlatform heart;
		ProgressBar health_bar;

		// animacja skurczu serca
		_Timer heart_anim;

		IrregularPlatform score;
		ProgressBar score_bar;

		/**
		 * Ekran smierci
		 */
		glText game_over;
		Button* retry_game;
		Button* return_to_menu;

		/**
		 * Wiadomości w intro od i do..
		 */
		IntroBackground* background;
		Character* hero;

		/**
		 * Miejsce na cutscene
		 */
		IrregularPlatform* cutscene_box;

	public:
		MessageRenderer(float, const Color&, const Color&, IntroBackground*);

		virtual void drawObject(Window*);
		virtual void catchEvent(const Event&);

		/**
		 * Callback od przycisków!
		 */
		void getCallback(Control* const &);

		bool popMessage();
		void addMessage(const Message&);

		/**
		 * Ekran śmierci
		 */
		void setScreen(usint _screen) {
			screen = _screen;
		}

		usint getActiveScreen() const {
			return screen;
		}

		/**
		 * Jeśli zamknięte - pokazywane jest
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
			if (cutscene_box) {
				delete cutscene_box;
			}
			delete retry_game;
			delete return_to_menu;
		}
	private:
		/**
		 * Odświeżanie
		 */
		void updateHUDControls();

		/**
		 * Otwieranie przerywnika pomiędzy scenami!
		 */
		void openCutscene(const Message&);
		void closeCutscene();

		void drawPlayerHUD(Window*);
		void drawIntroMessage(Window*);
		void drawDeathScreen(Window*);

		void drawBorder(Window*);
};

#endif /* HUD_HPP_ */
