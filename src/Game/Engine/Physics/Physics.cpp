/*
 * pEngine.cpp
 *
 *  Created on: 10-03-2013
 *      Author: mati
 */
#include <cmath>
#include <algorithm>

#include "../../Gameplay/Objects/Objects.hpp"

#include "Physics.hpp"

using namespace Physics;

//---------------------------

/** Odwrocenie kierunku */
usint Physics::invertDir(usint _dir) {
	switch (_dir) {
		case pEngine::DOWN:
			return pEngine::UP;
			
			//
		case pEngine::UP:
			return pEngine::DOWN;
			
			//
		case pEngine::LEFT:
			return pEngine::RIGHT;
			
			//
		case pEngine::RIGHT:
			return pEngine::LEFT;
	}
	return pEngine::NONE;
}

/** Sprawdzenie czy kierunek jest pionowy */
bool Physics::isHorizontalDir(usint _dir) {
	if (_dir == pEngine::LEFT || _dir == pEngine::RIGHT) {
		return true;
	}
	return false;
}

/** Unik ciala - odepchniecie w przeciwnym kierunku */
void Physics::dodgeBody(Body* _body, usint _dir, float _speed) {
	//
	switch (_dir) {
		case pEngine::RIGHT:
			_body->velocity.x = -_speed;
			break;
			
			//
		case pEngine::LEFT:
			_body->velocity.x = _speed;
			break;
			
			//
		case pEngine::UP:
			_body->velocity.y = _speed;
			break;
			
			//
		case pEngine::DOWN:
			_body->velocity.y = -_speed;
			break;
	}
	//
	_body->x += _body->velocity.x;
	_body->y += _body->velocity.y;
}

//---------------------------

pEngine::pEngine(const Rect<float>& _bounds, float _gravity_speed) :
				bounds(_bounds),
				gravity_speed(_gravity_speed),
				config(0),
				sleep_timer(0) {
	quadtree = new QuadTree(nullptr, _bounds, 0);
}

/** Dodawanie obiektu _force_register - rejestruj z ID sila */
void pEngine::insert(Body* body, bool _force_register) {
	if (!body) {
		return;
	}
	body->physics = this;
	
	/** Obiekty statyczne nie sią usuwane w czasie gry */
	if (_force_register || IS_SET(body->state, Body::STATIC)) {
		list.push_back(body);
	}
	quadtree->insert(body);
}

/** Usuwanie obiektu z quadtree */
bool pEngine::remove(Body* body) {
	return quadtree->remove(body);
}

/** Usypianie silnika miedzy klatkami 'slow motion' */
void pEngine::setSleep(usint _sleep_time) {
	sleep_timer.reset();
	sleep_timer.max_cycles_count = _sleep_time;
}

/** Test aktywnosci obiektu */
bool pEngine::isBodyActive(Body* object) {
	return object->state == Body::NONE;
}

/** Czy obiekt w tej chwili sie porusza? */
bool pEngine::isMoving(Body* body) {
	return !(body->velocity.x == 0 && body->velocity.y == 0);
}

/** Aktualizacja pedu obiektu */
void pEngine::updateBodyMovement(Body* object) {
	object->y += object->velocity.y;
	object->x += object->velocity.x;
}

/** Popychanie obiektu poziomo */
void pEngine::pushFromObject(Body* source, usint dir) {
	/** Popychanie obiektów */

	if (abs(source->velocity.x) > 0 && dir > 0) {
		Body* collision = source->collisions[dir - 1];
		
		/** Prawa kolizja */
		if (collision && isBodyActive(collision)) {
			float speed = -source->velocity.x * 0.85f;
			
			/** Popychanie obiektu */
			if (abs(collision->velocity.x) < abs(source->velocity.x)) {
				collision->velocity.x += speed;
			}
		}
	}
}

/** Odswiezanie swiata fizyki */
void pEngine::updateWorld() {
	if (IS_SET(config, Flags::PAUSE)) {
		return;
	}
	/** Uspienie */
	if (sleep_timer.active) {
		sleep_timer.tick();
		if (!sleep_timer.active) {
			sleep_timer.reset();
		}
		return;
	}
	
	/** Usuwanie wykasowanych obiektow! */
	visible_bodies.clear();
	
	/**
	 * Tworzenie quadtree!
	 * Optymalizacja:
	 * - Odswiezanie tylko widocznych!
	 */
	quadtree->update(active_range);
	quadtree->getBodiesAt(active_range, visible_bodies);
	
	/** Sprawdzenie kolizji! */
	checkCollisions(visible_bodies);
	
	/** Grawitacja! */
	if (IS_SET(config, Flags::GRAVITY_DISABLED)) {
		return;
	}
	
	for (usint i = 0; i < visible_bodies.size(); ++i) {
		Body* object = visible_bodies[i];
		
		/** Zywotnosc */
		if (object->life_timer.active) {
			object->life_timer.tick();
			if (!object->life_timer.active) {
				object->destroyed = true;
			}
		}
		
		/** Test obiektu  */
		if (!object
				|| !isBodyActive(
						object) || IS_SET(object->state, Body::FLYING)) {
			continue;
		}
		
		/** Popychanie obiektów */
		if (!IS_SET(object->state, Body::STATIC)
				&& abs(object->velocity.x) > 0) {
			/** Test kolizji w poziomie */
			pushFromObject(object, pEngine::RIGHT);
			pushFromObject(object, pEngine::LEFT);
		}
		
		/** Czy podlega grawitacji? */
		if (!(IS_SET(object->state, Body::BACKGROUND)
				&& (object->velocity.x != 0 || object->velocity.y != 0))) {
			/** Poruszanie się po platformie */
			Body* down_collision = object->collisions[DOWN - 1];
			
			if (down_collision && down_collision->velocity.x != 0) {
				if (!object->collisions[pEngine::LEFT - 1]
						&& !object->collisions[pEngine::RIGHT - 1]) {
					object->x += down_collision->velocity.x;
				}
			}
			
			/** Sila ciazenia */
			if (object->velocity.y < 20.f) {
				object->velocity.y += gravity_speed;
			}
			
			/** Sila tarcia / w locie tez chamuje */
			if (abs(object->velocity.x) > 0) {
				if (down_collision) {
					object->velocity.x *= down_collision->roughness;
				} else {
					object->velocity.x *= DEFAULT_ROUGHNESS;
				}
			}
		}
		
		/** Poruszenie obiektu */
		updateBodyMovement(object);
	}
}

void pEngine::checkCollisions(deque<Body*>& _bodies) {
	for (usint i = 0; i < _bodies.size(); ++i) {
		Body* source = _bodies[i];
		
		// Czyszczenie
		for (auto& coll : source->collisions) {
			coll = nullptr;
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
			
			/** Kolizje gora/dol */
			usint horizont_side = checkHorizontalCollision(source, target);
			if (horizont_side != NONE) {
				if (!IS_SET(target->state, (Body::HIDDEN | Body::BACKGROUND))) {
					if (horizont_side == DOWN) {
						/** Dol */
						source->y = target->y - source->h - target->velocity.y
								+ gravity_speed;
						
						/** Unoszenie sie ciala wraz z platforma */
						if (abs(source->velocity.y) < 0) {
							source->velocity.y = -target->velocity.y;
						} else {
							source->velocity.y = -source->velocity.y * 0.5f
									+ target->velocity.y;
						}
					} else {
						/** Gora */
						source->velocity.y = -source->velocity.y * 0.5f;
					}
					source->collisions[horizont_side - 1] = target;
				}
				source->catchCollision(this, horizont_side, target);
			}
			
			/** Kolizje lewo/prawo */
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

/** Sprawdzenie kolizji w poziomie! */
usint pEngine::checkVerticalCollision(Body* _body, Body* _body2) {
	if (_body2->x <= _body->x
			&& moveAndCheck(
					_body->velocity.x,
					-gravity_speed * 2,
					_body,
					_body2)) {
		return LEFT;
		/**
		 *
		 */
	} else if (_body2->x >= _body->x
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

/** Sprawdzenie kolizji w pionie! */
usint pEngine::checkHorizontalCollision(Body* _body, Body* _body2) {
	if (_body->x < _body2->x + _body2->w && _body->x + _body->w > _body2->x) {
		/**
		 *
		 */
		if (_body->y + _body->h + _body->velocity.y <= _body2->y + _body2->h
				&& _body->y + _body->h + _body->velocity.y >= _body2->y
				&& _body->y < _body2->y) {
			return DOWN;
			/**
			 *
			 */
		} else if (_body->y + _body->velocity.y <= _body2->y + _body2->h
				&& _body->y >= _body2->y + _body2->h && _body->y > _body2->y) {
			return UP;
			/**
			 *
			 */
		}
	}
	return NONE;
}

/** Poruszenie ciala po czym test kolizji oraz powrot */
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

/** Test kolizji prostokatow */
bool pEngine::collide(const Body* _body, const Body* _body2) const {
	if (_body->x + _body->w >= _body2->x && _body->x <= _body2->x + _body2->w
			&& _body->y + _body->h > _body2->y
			&& _body->y < _body2->y + _body2->h) {
		return true;
	}
	return false;
}

/* Czyszczenie! */
pEngine::~pEngine() {
	safe_delete<QuadTree>(quadtree);
}
