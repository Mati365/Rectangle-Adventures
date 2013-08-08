/*
 * pEngine.cpp
 *
 *  Created on: 10-03-2013
 *      Author: mati
 */
#include <cmath>
#include <algorithm>
#include <iostream>

#include "Physics.hpp"

using namespace Physics;

pEngine::pEngine(const Rect<float>& _bounds, float _gravity_speed) :
				bounds(_bounds),
				gravity_speed(_gravity_speed),
				pause(false),
				timer(0),
				sleep_time(0) {
	quadtree = new QuadTree(NULL, _bounds, 0);
}

/**
 * Usypianie silnika!
 */
void pEngine::setSleep(usint _sleep_time) {
	timer = 0;
	sleep_time = _sleep_time;
}

/**
 * Sprawdzenie aktywności obiektu!
 */
bool pEngine::isBodyActive(Body* object) {
	return !(IS_SET(object->state, Body::STATIC)
			|| IS_SET(object->state, Body::HIDDEN)
			|| IS_SET(object->state, Body::BACKGROUND)
			|| IS_SET(object->state, Body::BUFFERED));
}

/**
 * Odświeżanie świata!
 */
void pEngine::updateWorld() {
	if (pause) {
		return;
	}
	if (sleep_time != 0) {
		timer++;
		if (timer > sleep_time) {
			timer = 0;
		} else {
			return;
		}
	}
	
	/**
	 * Usuwanie wykasowanych obiektów!
	 */
	visible_bodies.clear();
	
	/**
	 * Tworzenie quadtree!
	 * Optymalizacja:
	 * - Odświeżanie tylko widocznych!
	 */
	quadtree->update(active_range);
	//quadtree->clear();
	//quadtree->insertGroup(&list);
	quadtree->getBodiesAt(active_range, visible_bodies);
	
	/**
	 * Sprawdzenie kolizji!
	 */
	checkCollisions(visible_bodies);
	
	/**
	 * Grawitacja!
	 */
	for (usint i = 0; i < visible_bodies.size(); ++i) {
		Body* object = visible_bodies[i];
		//
		if (!object || !isBodyActive(object)) {
			continue;
		}
		/**
		 * Poruszanie się po platformie
		 */
		Body* down_collision = object->collisions[DOWN - 1];
		if (down_collision && down_collision->velocity.x != 0) {
			object->x += down_collision->velocity.x;
		}
		
		/**
		 * Żywotność
		 */
		if (object->max_lifetime != 0) {
			object->lifetime++;
			if (object->lifetime >= object->max_lifetime) {
				object->destroyed = true;
				object->max_lifetime = 0;
			}
		}
		
		/**
		 * Grawitacja
		 */
		if (object->velocity.y < 20.f) {
			object->velocity.y += gravity_speed;
		}
		
		/**
		 * Siła tarcia
		 */
		if (abs(object->velocity.x) > 0) {
			object->velocity.x *= 0.85f;
		}
		object->y += object->velocity.y;
		object->x += object->velocity.x;
	}
}

void pEngine::checkCollisions(deque<Body*>& _bodies) {
	for (usint i = 0; i < _bodies.size(); ++i) {
		Body* source = _bodies[i];

		// Czyszczenie
		for (auto*& col : source->collisions) {
			col = NULL;
		}

		// Statyszne obiekty są omijane
		if (!isBodyActive(source)) {
			continue;
		}
		for (usint j = 0; j < _bodies.size(); ++j) {
			Body* target = _bodies[j];
			if (source->destroyed || j == i
					|| IS_SET(source->state, Body::STATIC)
					|| (source->layer != target->layer
							&& !IS_SET(target->state, Body::STATIC))) {
				continue;
			}

			/**
			 * Kolizje Góra/ Dół
			 */
			usint horizont_side = checkHorizontalCollision(source, target);
			if (horizont_side != NONE) {
				if (!IS_SET(target->state, (Body::HIDDEN | Body::BACKGROUND))) {
					if (horizont_side == DOWN) {
						/**
						 * Dół
						 */
						source->y = target->y - source->h - target->velocity.y
								+ gravity_speed;
						/**
						 *
						 */
						if (abs(source->velocity.y) < 0) {
							source->velocity.y = -target->velocity.y;
						} else {
							source->velocity.y = -source->velocity.y * 0.5f
									+ target->velocity.y;
						}
					} else {
						/**
						 * Góra
						 */
						source->velocity.y = -source->velocity.y * 0.5f;
					}
					source->collisions[horizont_side - 1] = target;
				}
				source->catchCollision(this, horizont_side, target);
			}

			/**
			 * Kolizje Lewo
			 */
			usint vertical_side = checkVerticalCollision(source, target);
			if (vertical_side != NONE) {
				if (!IS_SET(target->state, (Body::HIDDEN | Body::BACKGROUND))) {
					source->velocity.x = -source->velocity.x / 2;
					source->collisions[vertical_side - 1] = target;
				}
				source->catchCollision(this, vertical_side, target);
			}
		}
	}
}

/**
 * Sprawdzenie kolizji w poziomie!
 */
usint pEngine::checkVerticalCollision(Body* _body, Body* _body2) {
	if (_body2->x + _body2->w <= _body->x
			&& moveAndCheck(
					_body->velocity.x,
					-gravity_speed * 2,
					_body,
					_body2)) {
		return LEFT;
		/**
		 *
		 */
	} else if (_body2->x >= _body->x + _body->w
			&& moveAndCheck(
					_body->velocity.x,
					-gravity_speed * 2,
					_body,
					_body2)) {
		/**
		 *
		 */
		return RIGHT;
	}
	return NONE;
}

/**
 * Sprawdzenie kolizji w pionie!
 */
usint pEngine::checkHorizontalCollision(Body* _body, Body* _body2) {
	if (_body->x < _body2->x + _body2->w && _body->x + _body->w > _body2->x) {
		/**
		 *
		 */
		if (_body->y + _body->h + _body->velocity.y <= _body2->y + _body2->h
				&& _body->y + _body->h + _body->velocity.y >= _body2->y
				&& _body->y < _body2->y) {
			return DOWN;
		} else if (_body->y + _body->velocity.y <= _body2->y + _body2->h
				&& _body->y >= _body2->y + _body2->h && _body->y > _body2->y) {
			return UP;
		}
	}
	return NONE;
}

bool pEngine::moveAndCheck(float _x, float _y, Body* _body,
		const Body* _body2) {
	bool collision;
	
	_body->x += _x;
	_body->y += _y;
	collision = collide(_body, _body2);
	_body->x -= _x;
	_body->y -= _y;
	
	return collision;
}

bool pEngine::collide(const Body* _body, const Body* _body2) const {
	if (_body->x + _body->w >= _body2->x && _body->x <= _body2->x + _body2->w
			&& _body->y + _body->h > _body2->y
			&& _body->y < _body2->y + _body2->h) {
		return true;
	}
	return false;
}

pEngine::~pEngine() {
	/**
	 * Czyszczenie!
	 */
	if (quadtree) {
		delete quadtree;
	}
}
