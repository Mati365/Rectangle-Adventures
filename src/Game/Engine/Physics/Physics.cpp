/*
 * pEngine.cpp
 *
 *  Created on: 10-03-2013
 *      Author: mati
 */
#include <cmath>
#include <algorithm>

#include "Physics.hpp"

using namespace Physics;

pEngine::pEngine(const Rect<float>& _bounds, float _gravity_speed) :
		bounds(_bounds),
		gravity_speed(_gravity_speed),
		pause(false),
		timer(0),
		sleep_time(0) {
	quadtree = new QuadTree(NULL, _bounds, 0, 2);
}

void pEngine::setSleep(usint _sleep_time) {
	timer = 0;
	sleep_time = _sleep_time;
}

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
	bodies.clear();

	if (!to_remove.empty()) {
		for (auto iter = to_remove.begin(); iter != to_remove.end(); ++iter) {
			auto _pos = find(list.begin(), list.end(), *iter);
			if (_pos != list.end()) {
				quadtree->remove(*_pos);
				if (*_pos && (*_pos)->dynamically_allocated) {
					delete *_pos;
					*_pos = NULL;
				}
				list.erase(_pos);
			}
		}
		to_remove.clear();
	}
	/**
	 * Sprawdzenie kolizji!
	 */
	quadtree->clear();
	quadtree->insert(&list);
	quadtree->getLowestElements(bodies);
	for (usint i = 0; i < bodies.size(); ++i) {
		checkCollisions(bodies[i]);
	}
	/**
	 * Grawitacja!
	 */
	for (usint i = 0; i < list.size(); ++i) {
		Body* object = list[i];
		if (IS_SET(object->state, Body::STATIC)
				|| IS_SET(object->state, Body::HIDDEN)) {
			continue;
		}
		Body* down_collision = list[i]->collisions[DOWN - 1];
		if (down_collision && down_collision->velocity.x != 0) {
			list[i]->x += down_collision->velocity.x;
		}
		if (list[i]->velocity.y < 20.f) {
			list[i]->velocity.y += gravity_speed;
		}
		if (abs(object->velocity.x) > 0) {
			object->velocity.x *= 0.85f;
		}
		object->y += object->velocity.y;
		object->x += object->velocity.x;
		if (object->destroyed) {
			remove(list[i]);
		}
	}
}

void pEngine::checkCollisions(deque<Body*>& _bodies) {
	for (usint i = 0; i < _bodies.size(); ++i) {
		Body* source = _bodies[i];
		for (usint j = 0; j < 4; ++j) {
			source->collisions[j] = NULL;
		}
		if (IS_SET(_bodies[i]->state, Body::STATIC)) {
			continue;
		}
		for (usint j = 0; j < _bodies.size(); ++j) {
			Body* target = _bodies[j];
			if (source->destroyed || j == i
					|| (source->layer != target->layer
							&& !IS_SET(target->state, Body::STATIC)
							&& !IS_SET(source->state, Body::STATIC))
					|| (IS_SET(source->state, Body::STATIC)
							&& IS_SET(target->state, Body::STATIC))) {
				continue;
			}
			/**
			 * Kolizje Góra/ Dół
			 */
			usint horizont_side = checkHorizontalCollision(source, target);
			if (horizont_side != NONE) {
				if (!IS_SET(source->state, Body::HIDDEN)
						&& !IS_SET(target->state, Body::HIDDEN)) {
					if (horizont_side == DOWN) {
						/**
						 * Dół
						 */
						source->y = target->y - source->h - source->velocity.y
								+ gravity_speed;
						/**
						 *
						 */
						if (abs(source->velocity.y) * 0.5f < gravity_speed) {
							source->velocity.y = target->velocity.y;
						} else {
							source->velocity.y = -source->velocity.y * 0.5f
									+ target->velocity.y
									- (target->velocity.y < 0 ?
											gravity_speed : -gravity_speed);
						}
					} else {
						/**
						 * Góra
						 */
						source->y = target->y + target->h - source->velocity.y
								+ gravity_speed;
						source->velocity.y = 0;
					}
				}
				source->collisions[horizont_side - 1] = target;
				source->catchCollision(this, horizont_side, target);
				break;
			}
			/**
			 * Kolizje Lewo
			 */
			usint vertical_side = checkVerticalCollision(source, target);
			if (vertical_side != NONE) {
				if (!IS_SET(source->state, Body::HIDDEN)
						&& !IS_SET(target->state, Body::HIDDEN)) {
					source->velocity.x = -source->velocity.x / 2;
				}
				source->collisions[vertical_side - 1] = target;
				source->catchCollision(this, vertical_side, target);
				break;
			}
		}
	}
}

/**
 * Sprawdzenie kolizji w poziomie!
 */
usint pEngine::checkVerticalCollision(Body* _body, Body* _body2) {
	if (_body2->x + _body2->w <= _body->x
			&& moveAndCheck(_body->velocity.x, -gravity_speed * 2, _body,
							_body2)) {
		return LEFT;
		/**
		 *
		 */
	} else if (_body2->x >= _body->x + _body->w
			&& moveAndCheck(_body->velocity.x, -gravity_speed * 2, _body,
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
