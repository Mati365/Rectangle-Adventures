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
				draw_quad(_draw_quad) {
}

/**
 * Obiekty nie podlegające fizyce typu particle
 */
void ParalaxRenderer::addStaticObject(Body* _renderer) {
	static_objects.push_back(AllocKiller<Body>(_renderer));
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
				Rect<float>(cam.pos.x, cam.pos.y, WINDOW_WIDTH, WINDOW_HEIGHT));
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
	
	cout << list->size() << endl;

	glPushMatrix();
	glTranslatef(-cam.pos.x * ratio, -cam.pos.y * ratio, 0);
	if (draw_quad) {
		//physics->getQuadTree()->drawObject(NULL);
	}
	
	/**
	 * Obiekty poza ekranem wycinamy!
	 */
	for (usint i = 0; i < static_objects.size(); ++i) {
		static_objects[i]->drawObject(_window);
	}
	/**
	 * Renderowanie obiektów podlegających fizyce
	 */
	for (usint i = 0; i < list->size(); ++i) {
		Body* body = (*list)[i];
		if (IS_SET(body->state, Body::HIDDEN)) {
			continue;
		}
		body->drawObject(_window);
	}
	
	glPopMatrix();
}
