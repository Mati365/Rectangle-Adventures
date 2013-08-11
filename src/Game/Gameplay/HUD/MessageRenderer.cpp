/*
 * MessageRenderer.cpp
 *
 *  Created on: 28-03-2013
 *      Author: mati
 */
#include "HUD.hpp"
#include "../Gameplay.hpp"

#include "../Screens/Screens.hpp"

#include "../../GUI/GUI.hpp"
#include "../../Tools/Logger.hpp"

using namespace GameScreen;
using namespace Gameplay;
using namespace GUI;

/**
 * Konstruktor inicjalizacja kontrolek
 */
MessageRenderer::MessageRenderer(float _height, const Color& _title_color,
		const Color& _contents_color, IntroBackground* _background) :
				height(_height),
				screen(HUD_SCREEN),
				//
				text(_contents_color, "", GLUT_BITMAP_HELVETICA_18, 18),
				title(_title_color, "", GLUT_BITMAP_HELVETICA_18, 18),
				border_color(255, 255, 255),
				background_color(0, 0, 0),

				// HUD
				health(
						12,
						WINDOW_HEIGHT - 27,
						Body::NONE,
						getShapePointer("health"),
						16),
				health_bar(
						Rect<float>(0, 0, 0, 0),
						oglWrapper::RED,
						MAX_LIVES,
						Control::VERTICAL),

				score(
						WINDOW_WIDTH - 150,
						WINDOW_HEIGHT - 30,
						Body::NONE,
						getShapePointer("score"),
						16),
				score_bar(
						Rect<float>(0, 0, 0, 0),
						oglWrapper::GREEN,
						MAX_SCORE,
						Control::VERTICAL),

				//
				game_over(
						oglWrapper::WHITE,
						"Game over",
						GLUT_BITMAP_HELVETICA_18,
						18),

				//
				background(_background),
				hero(NULL),
				cutscene_box(NULL) {
	retry_game = new Button(
			Rect<float>(
					WINDOW_WIDTH / 2 - 10 - 100,
					WINDOW_HEIGHT - 45,
					100,
					35),
			"Od nowa",
			this);
	retry_game->putCallback(Event::MOUSE_RELEASED, this);

	return_to_menu = new Button(
			Rect<float>(WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT - 45, 100, 35),
			"Do menu");
	return_to_menu->putCallback(Event::MOUSE_RELEASED, this);
}

/**
 * Otwieranie cutscenki poprzez:
 * + Przemieszczenie kamery gdzie poza plansza
 * + Dodawanie(jesli nie ma) platformy wysokosci i szerokosci ekranu
 * + Nakladanie na nia ksztaltu
 */
void MessageRenderer::openCutscene(const Message& msg) {
	if (!msg.cutscene) {
		logEvent(Logger::LOG_WARNING, "Brak cutsceny!");
		return;
	}
	if (cutscene_box) {
		delete cutscene_box;
	}
	cutscene_box = new IrregularPlatform(
			-2000,
			-2000,
			Body::STATIC,
			msg.cutscene);
	cutscene_box->fitToWidth(WINDOW_WIDTH);
	//
	ParalaxRenderer* paralax = dynamic_cast<ParalaxRenderer*>(background);

	paralax->getPhysics()->insert(cutscene_box);
	paralax->getCamera()->focus = cutscene_box;
}

/**
 * Zamykanie cutsceny
 */
void MessageRenderer::closeCutscene() {
	ParalaxRenderer* paralax = dynamic_cast<ParalaxRenderer*>(background);
	IrregularPlatform* platform =
			dynamic_cast<IrregularPlatform*>(paralax->getCamera()->focus);
	//
	if (paralax->getCamera()->focus == paralax->getHero()) {
		return;
	}
	// Czyszczenie!
	main_resource_manager.deleteResource(platform->getShape()->getID());
	cutscene_box->destroyed = true;

	paralax->getCamera()->focus = paralax->getHero();
	//
	logEvent(Logger::LOG_INFO, "Zwolniono zasoby cutsceny!");
}

/**
 * Obramowanie box'u!
 */
void MessageRenderer::addMessage(const Message& msg) {
	msgs.push_front(msg);
}

/**
 * Rysowanie obramowania HUDu
 */
void MessageRenderer::drawBorder(Window* _window) {
	/**
	 * Wypełnienie!
	 */
	oglWrapper::drawFillRect(
			SPACES,
			_window->getBounds()->y - height + SPACES,
			_window->getBounds()->x - SPACES * 2,
			height - SPACES * 2,
			oglWrapper::BLACK);

	/**
	 * Obramowanie!
	 */
	glPushAttrib(GL_ENABLE_BIT);
	glLineStipple(1, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	/**
	 * Optymalizacja!
	 */
	float x = SPACES, y = _window->getBounds()->y - height + SPACES, w =
			_window->getBounds()->x - SPACES * 2, h = height - SPACES * 2;
	glLineWidth(screen != HUD_SCREEN ? 3 : 2);
	//
	glBegin(GL_LINE_LOOP);
	glColor4ub(border_color.r, border_color.g, border_color.b, 150.f);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glColor4ub(border_color.r, border_color.g, border_color.b, 0.f);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
	//
	glPopAttrib();
	if (screen != HUD_SCREEN) {
		oglWrapper::drawFillRect(
				SPACES * 2,
				_window->getBounds()->y - height,
				title.getScreenLength() + 20,
				17,
				background_color);
	}
}

/**
 * DLA intro!
 */
bool MessageRenderer::popMessage() {
	/**
	 * Zamykanie intro!
	 */
	closeCutscene();
	if (msgs.empty()) {
		background->setState(IntroBackground::RESUME);
		screen = HUD_SCREEN;
		return false;
	}
	/**
	 * Jeśli otwarte..
	 */

	Message pop = msgs[msgs.size() - 1];
	msgs.pop_back();
	//
	openCutscene(pop);

	title.setString(pop.title, 0);
	text.setString(pop.text, 0);
	text.setHidden(true);
	//
	screen = INTRO_SCREEN;
	//
	background->setState(IntroBackground::PAUSE);
	return true;
}

/**
 * Eventy klawiszy!
 */
void MessageRenderer::catchEvent(const Event& _event) {
	switch (screen) {
		/**
		 *
		 */
		case INTRO_SCREEN:
			if (_event.type == Event::KEY_PRESSED) {
				if (!text.isHidden() && !text.isAnim() && _event.key == '*') {
					popMessage();
				}
			}
			break;

			/**
			 *
			 */
		case DEATH_SCREEN:
			retry_game->catchEvent(_event);
			return_to_menu->catchEvent(_event);
			break;
	}

}

void MessageRenderer::drawPlayerHUD(Window* _window) {
	if (!hero) {
		hero = background->getHero();
	}
	if (health_bar.w == 0) {
		health_bar.setBounds(
				SPACES * 3 + health.w + SPACES * 2,
				_window->getBounds()->y - 30,
				112,
				20);
		score_bar.setBounds(
				_window->getBounds()->x - 112 - SPACES * 2,
				_window->getBounds()->y - 30,
				112,
				20);
	}
	/**
	 * Pasek życia!
	 */
	health_bar.setValue(hero->getStatus()->health);

	health.drawObject(NULL);
	health_bar.drawObject(NULL);
	/**
	 * Punkty!
	 */
	score_bar.setValue(hero->getStatus()->score);

	score.drawObject(NULL);
	score_bar.drawObject(NULL);
}

/**
 * Intro, prostokąt na dole ekranu wyświetla dialogi!
 */
void MessageRenderer::drawIntroMessage(Window* _window) {
	/**
	 * Kontrola animacji!
	 */
	if (!title.isAnim() && !text.isAnim()) {
		if (background->getState() == IntroBackground::PAUSE) {
			glText t(
					oglWrapper::YELLOW,
					"Aby kontynuowac wcisnij [SPACJE]!",
					GLUT_BITMAP_HELVETICA_18,
					18);
			t.printText(
					_window->getBounds()->x / 2 - t.getScreenLength() / 2,
					90);
		} else {
			popMessage();
		}
	} else if (text.isHidden()
			&& title.getRenderLength() == title.getString()->length()) {
		text.setHidden(false);
	}
	/**
	 * Wyświetlanie INTRO/OUTRO
	 */
	title.printText(SPACES * 2 + 5, _window->getBounds()->y - height + 12);
	text.printText(SPACES * 2 + 2, _window->getBounds()->y - 23 + 10);
}

/**
 * Ekran śmierci
 */
void MessageRenderer::drawDeathScreen(Window*) {
	game_over.printText(WINDOW_WIDTH / 2 - game_over.getScreenLength() / 2, 80);
	//
	retry_game->drawObject(NULL);
	return_to_menu->drawObject(NULL);
}

/**
 * Odbieranie callbacku z przycisków w menu!
 */
void MessageRenderer::getCallback(Control* const & control) {
	logEvent(Logger::LOG_INFO, "Otrzymano event w ekranie śmierci!");
	//
	if (control == return_to_menu) {
		delete menu;
		menu = new Menu();
		//
		active_screen = menu;
	}
}

void MessageRenderer::drawObject(Window* _window) {
	if (msgs.size() > 0 && screen != INTRO_SCREEN) {
		popMessage();
		//
		screen = INTRO_SCREEN;
	}
	if (screen != DEATH_SCREEN) {
		drawBorder(_window);
	}
	//
	switch (screen) {
		/**
		 *
		 */
		case INTRO_SCREEN:
			drawIntroMessage(_window);
			break;

			/**
			 *
			 */
		case HUD_SCREEN:
			drawPlayerHUD(_window);
			break;

			/**
			 *
			 */
		case DEATH_SCREEN:
			drawDeathScreen(_window);
			break;
	}
}

