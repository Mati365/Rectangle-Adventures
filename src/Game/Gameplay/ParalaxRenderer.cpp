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
void ParalaxRenderer::addStaticObject(Renderer* _renderer) {
	static_objects.push_back(AllocKiller<Renderer>(_renderer));
}

void ParalaxRenderer::drawObject(Window* _window) {
	if (!map) {
		return;
	}
	pEngine* physics = map->physics;
	if (state != PAUSE) {
		physics->updateWorld();
	}
	cam.updateCam(_window);

	const Vector<usint>* bounds = _window->getBounds();
	deque<Body*>* list = physics->getList();

	glPushMatrix();
	glTranslatef(-cam.pos.x * ratio, -cam.pos.y * ratio, 0);
	if (draw_quad) {
		physics->getQuadTree()->drawObject(
		NULL);
	}
	for (usint i = 0; i < list->size(); ++i) {
		if (IS_SET((*list)[i]->state, Body::HIDDEN)
				|| (IS_SET((*list)[i]->state, Body::STATIC)
						&& (*list)[i]->x - cam.pos.x > bounds->x
						&& (*list)[i]->y - cam.pos.y > bounds->y)) {
			continue;
		}
		(*list)[i]->drawObject(_window);
	}
	for (usint i = 0; i < static_objects.size(); ++i) {
		static_objects[i]->drawObject(_window);
	}
	glPopMatrix();
}
