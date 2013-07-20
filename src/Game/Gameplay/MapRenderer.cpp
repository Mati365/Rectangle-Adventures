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
		ParalaxRenderer(_hero, 1.f, true, _map),
		msg(50, Color(0, 128, 255), Color(255, 255, 255), this),
		hero(dynamic_cast<Character*>(_hero)),
		hud_enabled(true) {
}

/**
 * Dodawanie tła gry!
 */
ParalaxRenderer* MapRenderer::addToParalax(MapINFO* _paralax, float _ratio,
	Body* _body) {
	if (map->physics) {
		ParalaxRenderer* renderer = new ParalaxRenderer(_body, _ratio, false,
				_paralax);
		paralax_background.push_front(renderer);
		return renderer;
	}
	return NULL;
}

void MapRenderer::catchEvent(const Event& _event) {
	if (!hero) {
		return;
	}
	switch (_event.type) {
		case Event::KEY_PRESSED:
			if (state == IntroBackground::PAUSE) {
				msg.catchEvent(_event);
			} else {
				if (_event.key == 'w') {
					hero->jump(10.f);
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
		case SNOWING: {
			SnowEmitter* snow = new SnowEmitter(
					Rect<float>(0, 20, WINDOW_WIDTH, 0));
			snow->setFocus(&cam.pos);
			addStaticObject(snow);
		}
			break;
	}
}
void MapRenderer::setHero(Character* _hero) {
	hero = _hero;
	cam.focus = hero;
}

void MapRenderer::drawObject(Window* _window) {
	/**
	 * Z powodu zje**nych sterowniów GPU intel'a..
	 * glScissor() musi zostac wylaczone..
	 */
	for (usint i = 0; i < paralax_background.size(); ++i) {
		paralax_background[i]->drawObject(_window);
	}
	ParalaxRenderer::drawObject(_window);
	/**
	 * Elementy HUDu
	 */
	if (hud_enabled) {
		msg.drawObject(_window);
	}
}

MapRenderer::~MapRenderer() {
	for (usint i = 0; i < paralax_background.size(); ++i) {
		delete paralax_background[i];
	}
}

