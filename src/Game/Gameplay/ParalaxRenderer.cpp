/*
 * ParalaxRenderer.cpp
 *
 *  Created on: 14-04-2013
 *      Author: mateusz
 */
#include "Gameplay.hpp"

using namespace Gameplay;

ParalaxRenderer::ParalaxRenderer(Body* _target, float _ratio, bool _draw_quad,
		MapINFO* _map) :
				map(_map),
				cam(_target),
				ratio(_ratio),
				draw_quad(_draw_quad),
				rotate(true),
				shake_timer(170) {
	shake_timer.active = false;
}

/**
 * Obiekty nie podlegające fizyce typu particle
 */
void ParalaxRenderer::addStaticObject(Body* _renderer) {
	static_objects.push_back(_renderer);
}

/**
 * Potrząsanie ekranem
 */
void ParalaxRenderer::shake() {
	shake_timer.reset();
	//
	playResourceSound(
			getIntRandom<int>(0, 2) == 1 ?
					EARTH_QUAKE_SOUND_1 : EARTH_QUAKE_SOUND_2);
}

/**
 * Rysowanie paralaksy
 */
void ParalaxRenderer::drawObject(Window* _window) {
	if (!map) {
		return;
	}
	/**
	 * Odświeżanie fizyki
	 */
	pEngine* physics = map->physics;
	if (draw_quad) {
		physics->setActiveRange(
				Rect<float>(
						cam.pos.x * ratio,
						cam.pos.y * ratio,
						WINDOW_WIDTH,
						WINDOW_HEIGHT));
		physics->updateWorld();
	}
	
	/**
	 * Odświeżanie kamery
	 */
	cam.updateCam(_window);
	
	/**
	 * Lista aktualnie widocznych elementów!
	 */
	deque<Body*>* list = physics->getVisibleBodies();
	
	glPushMatrix();
	/**
	 * Transformacja kamery
	 */
	if (rotate) {
		glRotatef(sin(cam.focus->x / WINDOW_WIDTH * 2) * -8.f, 0.f, 0.f, 1.f);
	}
	float _x = -cam.pos.x * ratio, _y = -cam.pos.y * ratio;
	if (shake_timer.active) {
		shake_timer.tick();
		//
		float prop = (float) shake_timer.cycles_count
				/ (float) shake_timer.max_cycles_count;
		_x -= (float) getIntRandom<int>(3, 10) * sin(prop * 45);
		_y += (float) getIntRandom<int>(3, 10) * sin(prop * 85);
	}
	glTranslatef(_x, _y, 0);

	/**
	 * Rysowanie quadtree
	 */
	if (draw_quad) {
		physics->getQuadTree()->drawObject(NULL);
	}

	/**
	 * Obiekty poza ekranem wycinamy!
	 */
	for (auto& obj : static_objects) {
		obj->drawObject(_window);
	}
	/**
	 * Renderowanie obiektów podlegających fizyce
	 */
	for (usint i = 0; i < list->size(); ++i) {
		Body* body = (*list)[i];
		if (IS_SET(body->state, Body::HIDDEN) || body == cam.focus) {
			continue;
		}
		body->drawObject(_window);
	}
	/**
	 * Renderowanie focusa na końcu
	 */
	cam.focus->drawObject(NULL);
	glPopMatrix();
}
