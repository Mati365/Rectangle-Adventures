/*
 * MessageRenderer.cpp
 *
 *  Created on: 28-03-2013
 *      Author: mati
 */
#include "HUD.hpp"
#include "../Gameplay.hpp"

#include "../Screens/Screens.hpp"
#include "../LevelManager.hpp"

#include "../../GUI/GUI.hpp"
#include "../../Tools/Logger.hpp"

using namespace GameScreen;
using namespace Gameplay;
using namespace GUI;

/** Konstruktor inicjalizacja kontrolek */
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
				hud_temperature(432), // temperatura do ustawienia
				heart(12, 17, Body::NONE, nullptr, HEART_ICON_WIDTH),
				health_bar(
						Rect<float>(
								SPACES * 3 + HEART_ICON_WIDTH + SPACES * 2,
								16,
								62,
								16),
						oglWrapper::RED,
						MAX_LIVES,
						Control::VERTICAL),
				
				heart_anim(3),
				
				score(0, 16, Body::NONE, nullptr, SCORE_ICON_WIDTH),
				score_bar(
						Rect<float>(
								health_bar.x + health_bar.w + SPACES * 8,
								16,
								62,
								16),
						oglWrapper::GREEN,
						max_score,
						Control::VERTICAL),
				
				retry_hud(
						Rect<float>(
								screen_bounds.x - 22 - SPACES * 2,
								16,
								16,
								score_bar.y + score_bar.h),
						"Od nowa",
						nullptr,
						false,
						this),
				
				game_over(
						oglWrapper::WHITE,
						"Game over",
						GLUT_BITMAP_HELVETICA_18,
						18),
				
				//
				background(_background),
				hero(nullptr),
				cutscene_box(nullptr) {
	retry_game = new Button(
			Rect<float>(
					screen_bounds.x / 2 - 10 - 100,
					screen_bounds.y - 45,
					100,
					35),
			"Od nowa",
			nullptr,
			this);
	retry_game->putCallback(Event::MOUSE_RELEASED, this);
	
	return_to_menu = new Button(
			Rect<float>(
					screen_bounds.x / 2 + 10,
					screen_bounds.y - 45,
					100,
					35),
			"Do menu");
	return_to_menu->putCallback(Event::MOUSE_RELEASED, this);
	
	/** Dlugosc trawnia bicia serca */
	heart_anim.sleep_beetwen_cycle = 50;
	heart_anim.loop = true;
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
	cutscene_box->fitToWidth(screen_bounds.x);
	
	//
	ParalaxRenderer* paralax = dynamic_cast<ParalaxRenderer*>(background);
	
	paralax->getPhysics()->insert(cutscene_box);
	Camera::getFor(cutscene_box);
}

/** Zamykanie cutsceny */
void MessageRenderer::closeCutscene() {
	ParalaxRenderer* paralax = dynamic_cast<ParalaxRenderer*>(background);
	IrregularPlatform* platform =
			dynamic_cast<IrregularPlatform*>(Camera::getFor().getFocus());
	//
	if (!platform || Camera::getFor().getFocus() == paralax->getHero()) {
		return;
	}
	
	// Czyszczenie!
	main_resource_manager.deleteResource(platform->getShape()->getID());
	cutscene_box->destroyed = true;
	
	Camera::getFor(paralax->getHero());
	//
	logEvent(Logger::LOG_INFO, "Zwolniono zasoby cutsceny!");
}

/** Obramowanie box'u! */
void MessageRenderer::addMessage(const Message& msg) {
	msgs.push_front(msg);
}

/** Rysowanie obramowania HUDu */
void MessageRenderer::drawBorder(Window* _window) {
	if (screen == INTRO_SCREEN) {
		/** Wypelnienie wnetrza hudu */
		oglWrapper::drawFillRect(
				SPACES,
				screen_bounds.y - height + SPACES,
				screen_bounds.x - SPACES * 2,
				height,
				oglWrapper::BLACK);
		
		/** Optymalizacja poprzez nie uzycia oglWrapper::drawRect */
		float x = SPACES, y = screen_bounds.y - height + SPACES, w =
				screen_bounds.x - SPACES * 2, h = height - SPACES * 2;
		
		oglWrapper::beginStroke(0xAAAA);
		glLineWidth(screen != HUD_SCREEN ? 3 : 2);
		
		glBegin(GL_LINE_LOOP);
		
		glColor4ub(border_color.r, border_color.g, border_color.b, 150.f);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		
		glColor4ub(border_color.r, border_color.g, border_color.b, 0.f);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		
		glEnd();
		oglWrapper::endStroke();
		oglWrapper::drawFillRect(
				SPACES * 2,
				screen_bounds.y - height,
				title.getScreenLength() + 20,
				17,
				background_color);
	}
}

/** Kolejkowanie nastepnej wiadomosci */
bool MessageRenderer::popMessage() {
	/** Zamykanie intro jesli nie ma wiadomosci */
	closeCutscene();
	if (msgs.empty()) {
		background->setState(IntroBackground::RESUME);
		screen = HUD_SCREEN;
		return false;
	}
	
	/** A jesli sa.. otwieranie od nowa */
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

/** Eventy klawiatury */
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

/** Odswiezanie kontrolek HUDa */
void MessageRenderer::updateHUDControls() {
	/** Sprawdzenie temperatury textur i podmiana ewentualnie */
	usint _actual_temperature =
			ResourceFactory::getInstance(nullptr).getTextureTemperature();
	if (hud_temperature != _actual_temperature) {
		hud_temperature = _actual_temperature;
		
		// Serce
		heart.setShape(
				ResourceFactory::getInstance(nullptr).getTexture(
						ResourceFactory::HEALTH,
						pEngine::NONE));
		heart.fitToWidth(HEART_ICON_WIDTH);
		health_bar.setColor(*heart.getShape()->getMainColor());
		health_bar.getColor()->a = 208;
		
		// Punkt
		score.setShape(
				ResourceFactory::getInstance(nullptr).getTexture(
						ResourceFactory::SCORE,
						pEngine::NONE));
		
		score.fitToWidth(
				hud_temperature == ResourceFactory::ICY ?
						SCORE_ICON_WIDTH / 1.5f : SCORE_ICON_WIDTH);
		score_bar.setColor(*score.getShape()->getMainColor());
		score_bar.getColor()->a = 208;
	}
	
	/** Animacja bicia serca */
	heart_anim.tick();
	heart.fitToWidth(
			(float) HEART_ICON_WIDTH * ((float) heart_anim.cycles_count + 1)
					/ (float) heart_anim.max_cycles_count);
	if (!heart_anim.active) {
		//
		heart_anim.reset();
	}
	
	/** Aktualizacja pozycji HUDa nad graczem */
	Rect<float>* cam_pos = Camera::getFor().getPos();
	float ratio = 2.f - game->getMapRenderer()->getRatio();
	
	score_bar.x = hero->x * ratio - hero->w / 2 - cam_pos->x - score_bar.w / 2
			+ hero->velocity.x * 2;
	score_bar.y = hero->y * ratio - cam_pos->y - 120 - hero->velocity.y / 2;
	
	score.x = score_bar.x - score.w - SPACES * 2;
	score.y = score_bar.y - score.h / 2 + score_bar.h / 2;
	
	health_bar.x = score_bar.x;
	health_bar.y = score.y - SPACES - health_bar.h;
	
	heart.x = score.x + score.w / 2 - heart.w / 2;
	heart.y = health_bar.y;
	
	/** Aktualizacja wartosci na paskach HUDa */
	score_bar.setMaxValue(max_score);
	score_bar.setValue(hero->getStatus()->score);

	health_bar.setValue(hero->getStatus()->health);
}

/** Rysowanie HUDa */
void MessageRenderer::drawPlayerHUD(Window* _window) {
	if (!hero) {
		hero = background->getHero();
	}
	updateHUDControls();
	
	/** Rysowanie glownego HUDu */
	score.drawObject(nullptr);
	score_bar.drawObject(nullptr);
	
	heart.drawObject(nullptr);
	health_bar.drawObject(nullptr);
	
	if (!retry_hud.getIcon()) {
		retry_hud.setIcon(getShapePointer("retry_shape"));
	}
	retry_hud.drawObject(nullptr);
	
	/** Linia laczaca gracza z hudem */
	oglWrapper::beginStroke(0xA0A0);
	
	glLineWidth(2.f);
	glBegin(GL_LINE_STRIP);
	
	glColor4f(1.f, 1.f, 1.f, 1.f);
	glVertex2f(score_bar.x, score_bar.y + 20);
	
	glColor4f(1.f, 1.f, 1.f, 0.2f);
	glVertex2f(score_bar.x, score_bar.y + 100);
	
	glEnd();
	
	oglWrapper::endStroke();
}

/** Rysowanie intro - prostokat na dole ekranu z tekstem */
void MessageRenderer::drawIntroMessage(Window* _window) {
	/** Kontrola animacji tytulu i tekstu */
	if (!title.isAnim() && !text.isAnim()) {
		if (background->getState() == IntroBackground::PAUSE) {
			glText t(
					oglWrapper::YELLOW,
					"Aby kontynuowac wcisnij [SPACJE]!",
					GLUT_BITMAP_HELVETICA_18,
					18);
			t.printText(screen_bounds.x / 2 - t.getScreenLength() / 2, 90);
		} else {
			popMessage();
		}
	} else if (text.isHidden()
			&& title.getRenderLength() == title.getString()->length()) {
		text.setHidden(false);
	}
	
	/** Rendering */
	title.printText(SPACES * 2 + 5, screen_bounds.y - height + 12);
	text.printText(SPACES * 2 + 2, screen_bounds.y - 23 + 10);
}

/** Ekran smierci - game over */
void MessageRenderer::drawDeathScreen(Window*) {
	game_over.printText(
			screen_bounds.x / 2 - game_over.getScreenLength() / 2,
			80);
	//
	retry_game->drawObject(nullptr);
	return_to_menu->drawObject(nullptr);
}

/** Interfejs na callbacki z przyciskow */
void MessageRenderer::getCallback(Control* const & control) {
	logEvent(Logger::LOG_INFO, "Otrzymano event w ekranie śmierci!");
	//
	if (control == return_to_menu) {
		safe_delete<Menu>(menu);
		menu = new Menu();
		//
		active_screen = menu;
	} else if (control == retry_game) {
		LevelManager::getInstance().reloadMap();
	}
}

/** Renderowanie wszystkiego */
void MessageRenderer::drawObject(Window* _window) {
	if (msgs.size() > 0 && screen != INTRO_SCREEN) {
		popMessage();
		//
		screen = INTRO_SCREEN;
	}
	//
	switch (screen) {
		/**
		 *
		 */
		case INTRO_SCREEN:
			drawBorder(_window);
			drawIntroMessage(_window);
			break;
			
			/**
			 *
			 */
		case HUD_SCREEN:
			drawBorder(_window);
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

