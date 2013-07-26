/*
 * MessageRenderer.cpp
 *
 *  Created on: 28-03-2013
 *      Author: mati
 */
#include "HUD.hpp"
#include "../Gameplay.hpp"

#include "../../GUI/GUI.hpp"
#include "../../Tools/Logger.hpp"

using namespace Gameplay;
using namespace GUI;

MessageRenderer::MessageRenderer(float _height, const Color& _title_color,
									const Color& _contents_color,
									IntroBackground* _background) :
		height(_height),
		closed(false),
		text(_contents_color, "", GLUT_BITMAP_HELVETICA_18, 18),
		title(_title_color, "", GLUT_BITMAP_HELVETICA_18, 18),
		border_color(255, 255, 255),
		background_color(0, 0, 0),
		// HUD
		health(oglWrapper::WHITE, "Zdrowie:", GLUT_BITMAP_HELVETICA_18, 18),
		health_bar(Rect<float>(0, 0, 0, 0), oglWrapper::GREEN, MAX_LIVES,
					Control::VERTICAL),

		score(oglWrapper::WHITE, "Punkty:", GLUT_BITMAP_HELVETICA_18, 18),
		score_bar(Rect<float>(0, 0, 0, 0), oglWrapper::PURPLE, MAX_SCORE,
					Control::VERTICAL),
		//

		background(_background),
		hero(NULL),
		cutscene_box(NULL) {
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
	cutscene_box = new IrregularPlatform(-2000, -2000, Body::STATIC,
											msg.cutscene);
	cutscene_box->fitToWidth(WINDOW_WIDTH);
	//
	ParalaxRenderer* paralax = dynamic_cast<ParalaxRenderer*>(background);

	paralax->getPhysics()->insert(cutscene_box);
	paralax->getCamera()->focus = cutscene_box;
}

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
	paralax->getPhysics()->remove(cutscene_box);

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

void MessageRenderer::drawBorder(Window* _window) {
	/**
	 * Wypełnienie
	 */
	oglWrapper::drawFillRect(SPACES, _window->getBounds()->y - height + SPACES,
								_window->getBounds()->x - SPACES * 2,
								height - SPACES * 2, oglWrapper::BLACK);
	/**
	 * Obramówka
	 */
	glPushAttrib (GL_ENABLE_BIT);
	glLineStipple(1, 0xAAAA);
	glEnable (GL_LINE_STIPPLE);
	oglWrapper::drawRect(SPACES, _window->getBounds()->y - height + SPACES,
							_window->getBounds()->x - SPACES * 2,
							height - SPACES * 2, border_color, closed ?
									3 : 2);
	glPopAttrib();
	if (closed) {
		oglWrapper::drawFillRect(SPACES * 2, _window->getBounds()->y - height,
									title.getScreenLength() + 20, 17,
									background_color);
	}
}

bool MessageRenderer::popMessage() {
	static int _old_height;
	/**
	 * Zamykanie intro!
	 */
	closeCutscene();
	if (msgs.empty()) {
		height = _old_height;
		background->setState(IntroBackground::RESUME);
		closed = false;
		return false;
	}
	/**
	 * Jeśli otwarte..
	 */
	if (height != 100) {
		_old_height = height;
	}
	height = 100;

	Message pop = msgs[msgs.size() - 1];
	msgs.pop_back();
	//
	openCutscene(pop);

	title.setString(pop.title, 0);
	text.setString(pop.text, 0);
	text.setHidden(true);
	closed = true;
	//
	background->setState(IntroBackground::PAUSE);
	return true;
}

/**
 * Eventy klawiszy!
 */
void MessageRenderer::catchEvent(const Event& _event) {
	if (_event.type == Event::KEY_PRESSED) {
		if (!text.isHidden() && !text.isAnim() && _event.key == '*') {
			popMessage();
		}
	}
}

void MessageRenderer::drawPlayerHUD(Window* _window) {
	if (!hero) {
		hero = background->getHero();
	}
	if (health_bar.w == 0) {
		health_bar.setBounds(SPACES * 2 + 75,
								_window->getBounds()->y - height + 13, 152, 25);
		score_bar.setBounds(_window->getBounds()->x - 152 - SPACES * 2,
							_window->getBounds()->y - height + 13, 152, 25);
	}
	/**
	 * Pasek życia!
	 */
	health_bar.setValue(hero->getStatus()->health);
	health.printText(SPACES * 2, _window->getBounds()->y - height + 30);
	health_bar.drawObject(NULL);
	/**
	 * Punkty!
	 */
	score_bar.setValue(hero->getStatus()->score);
	score.printText(_window->getBounds()->x - 227,
					_window->getBounds()->y - height + 30);
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
			glText t(oglWrapper::YELLOW, "Aby kontynuowac wcisnij [SPACJE]!",
			GLUT_BITMAP_HELVETICA_18,
						18);
			t.printText(_window->getBounds()->x / 2 - t.getScreenLength() / 2,
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
	title.printText(SPACES * 2 + 5, _window->getBounds()->y - height + 13);
	text.printText(SPACES * 2, _window->getBounds()->y - height + 40);
}

void MessageRenderer::drawObject(Window* _window) {
	if (msgs.size() > 0 && !closed) {
		popMessage();
	}
	drawBorder(_window);
	if (closed) {
		drawIntroMessage(_window);
	} else {
		drawPlayerHUD(_window);
	}
}

