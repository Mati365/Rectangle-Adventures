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
			PAUSE, RESUME, STOP
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
class MessageRenderer: public Renderer, public EventListener {
	public:
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
		bool closed;

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
		glText health;
		ProgressBar health_bar;

		glText score;
		ProgressBar score_bar;

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

		bool popMessage();
		void addMessage(const Message&);

		virtual void drawObject(Window*);
		virtual void catchEvent(const Event&);

		/**
		 * Jeśli zamknięte - pokazywane jest
		 * intro.
		 */
		bool isClosed() const {
			return closed;
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
		}
	private:
		/**
		 * Otwieranie przerywnika pomiędzy scenami!
		 */
		void openCutscene(const Message&);
		void closeCutscene();

		void drawPlayerHUD(Window*);
		void drawIntroMessage(Window*);

		void drawBorder(Window*);
};

#endif /* HUD_HPP_ */
