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
	quadtree = new QuadTree(_bounds, 0, 3);
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
	quadtree->clear();

	if (!to_remove.empty()) {
		for (auto iter = to_remove.begin(); iter != to_remove.end(); ++iter) {
			auto _pos = find(list.begin(), list.end(), *iter);
			if (_pos != list.end()) {
				if (*_pos && (*_pos)->dynamically_allocated) {
					delete *_pos;
					*_pos = NULL;
				}
				list.erase(_pos);
			}
		}
		to_remove.clear();
	}

	quadtree->insert(&list);
	quadtree->getLowestElements(bodies);

	for (usint i = 0; i < bodies.size(); ++i) {
		checkCollisions(bodies[i]);
	}
	for (usint i = 0; i < list.size(); ++i) {
		if (IS_SET(list[i]->state, Body::STATIC)
				|| IS_SET(list[i]->state, Body::HIDDEN)) {
			continue;
		}
		if (list[i]->velocity.y < 20.f) {
			list[i]->velocity.y += gravity_speed;
		}
		if (abs(list[i]->velocity.x) > 0) {
			list[i]->velocity.x *= 0.85f;
		}
		list[i]->y += list[i]->velocity.y;
		list[i]->x += list[i]->velocity.x;
		if (list[i]->destroyed) {
			remove(list[i]);
		}
	}
}

void pEngine::checkCollisions(deque<Body*>& _bodies) {
	for (usint i = 0; i < _bodies.size(); ++i) {
		if (IS_SET(_bodies[i]->state, Body::STATIC)) {
			continue;
		}
		for (usint j = 0; j < _bodies.size(); ++j) {
			if (_bodies[i]->destroyed || j == i
					|| (_bodies[i]->layer != _bodies[j]->layer
							&& !IS_SET(_bodies[j]->state, Body::STATIC)
							&& !IS_SET(_bodies[i]->state, Body::STATIC))
					|| (IS_SET(_bodies[i]->state, Body::STATIC)
							&& IS_SET(_bodies[j]->state, Body::STATIC))) {
				continue;
			}
			/**
			 * Kolizje Góra/ Dół
			 */
			usint horizont_side = checkHorizontalCollision(
					_bodies[i], _bodies[j]);
			if (horizont_side != NONE) {
				if (!IS_SET(_bodies[i]->state, Body::HIDDEN)
						&& !IS_SET(_bodies[j]->state, Body::HIDDEN)) {
					if (horizont_side == DOWN) {
						/**
						 * Dół
						 */
						_bodies[i]->y = _bodies[j]->y - _bodies[i]->h
								+ gravity_speed;
						//_bodies[i]->x = _bodies[j]->velocity.x;
						/**
						 *
						 */
						if (abs(_bodies[i]->velocity.y) * 0.5f
								< gravity_speed) {
							_bodies[i]->velocity.y = _bodies[j]->velocity.y;
						} else {
							_bodies[i]->velocity.y = -_bodies[i]->velocity.y
									* 0.5f + _bodies[j]->velocity.y
									- (_bodies[j]->velocity.y < 0 ?
											gravity_speed : -gravity_speed);
						}
					} else {
						/**
						 * Góra
						 */
						_bodies[i]->y = _bodies[j]->y + _bodies[j]->h
								- _bodies[i]->velocity.y + gravity_speed;
						_bodies[i]->velocity.y = 0;
					}
				}
				_bodies[i]->catchCollision(this, horizont_side, _bodies[j]);
			}
			/**
			 * Kolizje Lewo
			 */
			usint vertical_side = checkVerticalCollision(
					_bodies[i], _bodies[j]);
			if (vertical_side != NONE) {
				if (!IS_SET(_bodies[i]->state, Body::HIDDEN)
						&& !IS_SET(_bodies[j]->state, Body::HIDDEN)) {
					_bodies[i]->velocity.x = -_bodies[i]->velocity.x / 2;
				}
				_bodies[i]->catchCollision(this, vertical_side, _bodies[j]);
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
					_body->velocity.x, -gravity_speed * 2, _body, _body2)) {
		return LEFT;
		/**
		 *
		 */
	} else if (_body2->x >= _body->x + _body->w
			&& moveAndCheck(
					_body->velocity.x, -gravity_speed * 2, _body, _body2)) {
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
