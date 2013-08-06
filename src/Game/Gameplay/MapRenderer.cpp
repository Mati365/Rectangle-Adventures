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

MapRenderer::MapRenderer(Body* _hero, MapINFO* _map) :
				ParalaxRenderer(_hero, 0.95f, true, _map),
				msg(45, Color(0, 128, 255), Color(255, 255, 255), this),
				hero(dynamic_cast<Character*>(_hero)),
				hud_enabled(true) {
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
				false,
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
	switch (_event.type) {
		case Event::KEY_PRESSED:
			if (state == IntroBackground::PAUSE) {
				msg.catchEvent(_event);
			} else if (hero) {
				if (_event.key == 'w') {
					hero->jump(7.f, false);
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
	 * glScissor - rzecz sporna dla GPU intel!
	 */
	shaders[WINDOW_SHADOW_SHADER]->begin();
	//
	for (usint i = 0; i < paralax_background.size(); ++i) {
		paralax_background[i]->drawObject(_window);
	}
	ParalaxRenderer::drawObject(_window);
	//
	shaders[WINDOW_SHADOW_SHADER]->end();
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

