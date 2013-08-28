/*
 * QuadTree.cpp
 *
 *  Created on: 05-03-2013
 *      Author: mati
 */
#include <map>
#include <iostream>

#include "Physics.hpp"

using namespace Physics;

/**
 * Konstruktor
 */
QuadTree::QuadTree(QuadTree* _parent, const Rect<float>& _rect, usint _level) :
				rect(_rect),
				level(_level),
				parent(_parent),
				NW(NULL),
				NE(NULL),
				SW(NULL),
				SE(NULL) {
}

/**
 * Rozdzielenie!
 */
void QuadTree::subdive() {
	if (NW) {
		return;
	}
	NW = new QuadTree(
			this,
			Rect<float>(rect.x, rect.y, rect.w / 2, rect.h / 2),
			level + 1);
	NE = new QuadTree(
			this,
			Rect<float>(rect.x + rect.w / 2, rect.y, rect.w / 2, rect.h / 2),
			level + 1);
	SW = new QuadTree(
			this,
			Rect<float>(rect.x, rect.y + rect.h / 2, rect.w / 2, rect.h / 2),
			level + 1);
	SE = new QuadTree(
			this,
			Rect<float>(
					rect.x + rect.w / 2,
					rect.y + rect.h / 2,
					rect.w / 2,
					rect.h / 2),
			level + 1);
}

/**
 * Odświeżanie
 */
void QuadTree::update(Rect<float>& _bounds) {
	if (!rect.intersect(_bounds)) {
		return;
	}
	for (usint i = 0; i < bodies.size(); ++i) {
		bool erase = false;
		Body* obj = bodies[i];
		//
		if (!obj) {
			erase = true;

		} else if (obj->destroyed) {
			if (!obj->with_observer) {
				safe_delete<Body>(obj);
			}
			erase = true;
		} else if (!IS_SET(obj->state, Body::STATIC)) {
			Rect<float> _rect = static_cast<Rect<float> >(*obj);
			//
			if (!rect.contains(_rect)) {
				if (parent) {
					parent->insertToSubQuad(obj, true);
				} else {
					insertToSubQuad(obj, false);
				}
				erase = true;
			}
		}
		if (erase) {
			bodies.erase(bodies.begin() + i);
			i--;
		}
	}
	if (NW) {
		NW->update(_bounds);
		NE->update(_bounds);
		SW->update(_bounds);
		SE->update(_bounds);
	}
}

/**
 * Rysowanie siatki
 */
void QuadTree::drawObject(Window*) {
	if (!NW || bodies.empty()) {
		return;
	}
	oglWrapper::drawRect(
			rect.x,
			rect.y,
			rect.w,
			rect.h,
			Color(level * 15, level * 15, level * 15, 255),
			(MAX_LAYER - level) * 2);
	if (NW) {
		NW->drawObject(NULL);
		NE->drawObject(NULL);
		SW->drawObject(NULL);
		SE->drawObject(NULL);
	}
}

/**
 * Dodawanie elementu!
 */
bool QuadTree::insertToSubQuad(Body* body, bool recursive) {
	if (!body) {
		return false;
	}
	if (!rect.contains(*body)) {
		if (recursive && parent) {
			return parent->insertToSubQuad(body, true);
		}
		return false;
	}

	/**
	 * Jeśli 1 lub więcej quadów ma ten sam element to
	 * wrzuca do rodzica!
	 */
	if (bodies.size() < 4 || body->w >= rect.w / 2 || body->h >= rect.h / 2) {
		bodies.push_back(body);
		return true;
	}
	if (!NW) {
		subdive();
	}

	/**
	 * Umieszczanie do dzieci
	 */
	if (NW->insertToSubQuad(body, false) || NE->insertToSubQuad(body, false)
			|| SE->insertToSubQuad(body, false)
			|| SW->insertToSubQuad(body, false)) {
		return true;
	} else {
		bodies.push_back(body);
		return true;
	}
	return false;
}

/**
 * Dodawanie całej grupy obiektów
 */
void QuadTree::insertGroup(deque<Body*>* bodies) {
	for (auto iter = bodies->begin(); iter != bodies->end();) {
		if ((*iter)->destroyed) {
			Body* body = (*iter);
			iter = bodies->erase(iter);
			//
			if (!body->with_observer) {
				safe_delete<Body>(body);
			}
		} else {
			insertToSubQuad(*iter++, false);
		}
	}
}

/**
 * Dodawanie
 */
void QuadTree::insert(Body* body) {
	insertToSubQuad(body, false);
}

/**
 * Usuwanie
 */
bool QuadTree::remove(Body* body) {
	for (auto iter = bodies.begin(); iter != bodies.end(); ++iter) {
		// Porównywanie adresów
		if (*iter == body) {
			if (!body->with_observer) {
				safe_delete<Body>(body);
			}
			bodies.erase(iter);
			return true;
		}
	}
	if (NW) {
		return NW->remove(body) || NE->remove(body) || SW->remove(body)
				|| SE->remove(body);
	}
	return false;
}

/**
 * Pobieranie elemntów z wycinka!
 */
void QuadTree::getBodiesAt(Rect<float>& _bounds, deque<Body*>& _bodies) {
	if (!rect.intersect(_bounds)) {
		return;
	}
	for (usint i = 0; i < bodies.size(); ++i) {
		Body* body = bodies[i];

		/** Jeśli obiekt mieści się w zasięgu widzialnego ekranu */
		if (_bounds.intersect(
				Rect<float>(body->x, body->y, body->w, body->h))) {

			/** Resetowanie obiektów */
			UNFLAG(body->state, Body::BUFFERED);

			/** A co jeśli obiekt jest przycięty na ekranie? */
			if (!IS_SET(body->state, Body::STATIC)
					&& (body->y + body->h >= _bounds.y + _bounds.h
							|| body->x + body->w >= _bounds.x + _bounds.w
							|| body->x <= _bounds.x || body->y <= _bounds.y)) {
				ADD_FLAG(body->state, Body::BUFFERED);
			}

			/** Dodawanie obiektu */
			_bodies.push_back(body);
		}
	}
	if (NW) {
		NW->getBodiesAt(_bounds, _bodies);
		NE->getBodiesAt(_bounds, _bodies);
		SW->getBodiesAt(_bounds, _bodies);
		SE->getBodiesAt(_bounds, _bodies);
	}
}

QuadTree::~QuadTree() {
	for (auto& obj : bodies) {
		if (obj && !obj->with_observer) {
			safe_delete<Body>(obj);
		}
	}
	if (NW) {
		safe_delete<QuadTree>(NW);
		safe_delete<QuadTree>(NE);
		safe_delete<QuadTree>(SW);
		safe_delete<QuadTree>(SE);
	}
}

