/*
 * MapRenderer.cpp
 *
 *  Created on: 16-03-2013
 *      Author: mati
 */
#include <GL/gl.h>
#include <GL/glut.h>

#include "Gameplay.hpp"
#include "Particle/Particle.hpp"

using namespace Gameplay;

#define DEFAULT_SHADOW_RADIUS 250

/**
 * Główny renderer mapy
 */
MapRenderer::MapRenderer(Body* _hero, MapINFO* _map) :
				ParalaxRenderer(_hero, 0.95f, true, _map),
				msg(45, Color(0, 128, 255), Color(255, 255, 255), this),
				hero(dynamic_cast<Character*>(_hero)),
				hud_enabled(true),
				main_shader_id(WINDOW_SHADOW_SHADER),
				shadow_radius(DEFAULT_SHADOW_RADIUS) {
}

/**
 * Dodawanie tła gry!
 */
ParalaxRenderer* MapRenderer::addToParalax(MapINFO* _paralax, float _ratio,
		Body* _body) {
	if (map->physics) {
		ParalaxRenderer* renderer = new ParalaxRenderer(
				_body,
				_ratio,
				true,
				_paralax);
		paralax_background.push_front(renderer);
		return renderer;
	}
	return NULL;
}

/**
 * Eventy otrzymywane z okna
 */
void MapRenderer::catchEvent(const Event& _event) {
	if (!hero) {
		return;
	}
	if (!IS_SET(msg.getScreen(), MessageRenderer::HUD_SCREEN)) {
		msg.catchEvent(_event);
		return;
	}
	switch (_event.type) {
		/**
		 *
		 */
		case Event::KEY_PRESSED:
			if (hero) {
				if (_event.key == 'w') {
					hero->jump(5.f, false);
				} else if (_event.key == 'a') {
					hero->move(-2.f, 0.f);
				} else if (_event.key == 'd') {
					hero->move(2.f, 0.f);
				}
			}
			break;

			/**
			 *
			 */
		case Event::MOUSE_CLICKED:
			break;
	}
}

/**
 * Dodawanie pogody
 * AllocKiller, brak wycieku!
 */
void MapRenderer::addWeather(usint _type) {
	switch (_type) {
		/**
		 * Śnieg
		 */
		case SNOWING: {
			SnowEmitter* snow = new SnowEmitter(
					Rect<float>(0, 20, WINDOW_WIDTH, 0));
			snow->setFocus(&cam.pos);
			//
			addStaticObject(snow);
		}
			break;

			/**
			 * Erupcja wulkanu
			 */
		case SHAKE:
			shake();
			break;
	}
}

/**
 * Ustawienie focusa kamery i bohatera
 */
void MapRenderer::setHero(Character* _hero) {
	hero = _hero;
	cam.focus = hero;
}

void MapRenderer::drawObject(Window* _window) {
	/**
	 * Sprawdzenie stanu gracza oraz dopasowanie do niego
	 * shaderu
	 */
	if (hero->isDead()) {
		switch (main_shader_id) {
			/**
			 *
			 */
			case WINDOW_DEATH_SHADER:
				shadow_radius += 0.7f;
				if (shadow_radius >= DEFAULT_SHADOW_RADIUS) {
					shadow_radius = DEFAULT_SHADOW_RADIUS;
				}
				break;

				/**
				 *
				 */
			case WINDOW_SHADOW_SHADER:
				if (!IS_SET(hero->getAction(), Character::BLOODING)) {
					shadow_radius -= 0.7f;
					if (shadow_radius <= 50) {
						if (hud_enabled
								&& msg.getScreen()
										!= MessageRenderer::DEATH_SCREEN) {
							/**
							 * A co jeśli jest może checkpoint?
							 */
							if (hero->isCheckpointAvailable()) {
								hero->recoverFromCheckpoint(map->physics);
								//
								main_shader_id = WINDOW_SHADOW_SHADER;
								shadow_radius = DEFAULT_SHADOW_RADIUS;
								//
								break;
							} else {
								/**
								 * Nie ma checkpointu ;(
								 */
								msg.enableDeathScreen();
							}
						}
						main_shader_id = WINDOW_DEATH_SHADER;
					}
				}
				break;
		}
	} else if (hero->isDrawingBlood()) {
		main_shader_id = WINDOW_DEATH_SHADER;
	} else {
		main_shader_id = WINDOW_SHADOW_SHADER;
	}
	/**
	 * Rysowanie
	 */
	shaders[main_shader_id]->begin();
	shaders[main_shader_id]->setUniform2f(
			"center",
			hero->x + hero->w / 2 - cam.pos.x,
			hero->y + hero->h / 2 - cam.pos.y);
	shaders[main_shader_id]->setUniform1f("radius", shadow_radius);

	//
	for (usint i = 0; i < paralax_background.size(); ++i) {
		paralax_background[i]->drawObject(_window);
	}
	ParalaxRenderer::drawObject(_window);
	//
	shaders[main_shader_id]->end();
	/**
	 * Elementy HUDu
	 */
	if (hud_enabled) {
		msg.drawObject(_window);
	}
}

MapRenderer::~MapRenderer() {
	for (auto* paralax : paralax_background) {
		delete paralax;
	}
}

