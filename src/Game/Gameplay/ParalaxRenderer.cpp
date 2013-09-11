/*
 * ParalaxRenderer.cpp
 *
 *  Created on: 14-04-2013
 *      Author: mateusz
 */
#include "../Resources/Data/SoundManager.hpp"

#include "Gameplay.hpp"

using namespace Gameplay;

ParalaxRenderer::ParalaxRenderer(Body* _target, float _ratio, MapINFO* _map) :
				config(0),
				map(_map),
				ratio(_ratio),
				shake_timer(170) {
	shake_timer.active = false;
	//
	Camera::getFor(_target);
}

/**
 * Obiekty nie podlegające fizyce typu particle
 */
void ParalaxRenderer::addStaticObject(Body* _renderer) {
	static_objects.push_back(AllocKiller<Body>(_renderer));
}

/**
 * Potrząsanie ekranem
 */
void ParalaxRenderer::shake() {
	shake_timer.reset();
	//
	SoundManager::getInstance().playResourceSound(
			getIntRandom<int>(0, 2) == 1 ?
					SoundManager::EARTH_QUAKE_SOUND_1 :
					SoundManager::EARTH_QUAKE_SOUND_2);
}

/**
 * Rysowanie paralaksy
 */
void ParalaxRenderer::drawObject(Window* _window) {
	if (!map) {
		return;
	}
	Camera& cam = Camera::getFor(nullptr);
	
	/** Odświeżanie fizyki */
	pEngine* physics = map->physics;
	if (!physics->getList()->empty()) {
		physics->setActiveRange(
				Rect<float>(
						cam.getFocus()->x - DEFAULT_SHADOW_RADIUS
								+ cam.getFocus()->velocity.x,
						cam.getFocus()->y - DEFAULT_SHADOW_RADIUS
								+ cam.getFocus()->velocity.y,
						DEFAULT_SHADOW_RADIUS * 2
								+ cam.getFocus()->velocity.x * 2,
						DEFAULT_SHADOW_RADIUS * 2
								+ cam.getFocus()->velocity.y * 2));
		physics->updateWorld();
	}
	
	/** Odświeżanie kamery */
	cam.updateCam(_window);
	
	/** Lista aktualnie widocznych elementów! */
	deque<Body*>* list = physics->getVisibleBodies();
	
	glPushMatrix();
	
	/** Transformacja kamery */
	if (IS_SET(config, ROTATION)) {
		glRotatef(
				sin(cam.getFocus()->x / screen_bounds.x * 2) * -9.f,
				0.f,
				0.f,
				1.f);
	}
	float _x = -cam.getPos()->x * ratio, _y = -cam.getPos()->y * ratio;
	if (shake_timer.active) {
		shake_timer.tick();
		//
		float prop = (float) shake_timer.cycles_count
				/ (float) shake_timer.max_cycles_count;
		_x -= (float) getIntRandom<int>(3, 10) * sin(prop * 45);
		_y += (float) getIntRandom<int>(3, 10) * sin(prop * 85);
	}
	glTranslatef(_x, _y, 0);
	
	/**  Rysowanie quadtree  */
	if (IS_SET(config, DRAW_QUAD)) {
		physics->getQuadTree()->drawObject(NULL);
	}
	
	/** Obiekty poza ekranem wycinamy! */
	for (auto& obj : static_objects) {
		obj->drawObject(_window);
	}
	
	/** Renderowanie obiektów podlegających fizyce  */
	for (usint i = 0; i < list->size(); ++i) {
		Body* body = (*list)[i];
		if (IS_SET(body->state, Body::HIDDEN) || body == cam.getFocus()) {
			continue;
		}
		body->drawObject(_window);
	}
	
	/** Renderowanie focusa na końcu  */
	if (!IS_SET(config, PARALLAX)
			&& !IS_SET(cam.getFocus()->state, Body::HIDDEN)) {
		cam.getFocus()->drawObject(NULL);
	}
	glPopMatrix();
}
