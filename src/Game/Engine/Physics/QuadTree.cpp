/*
 * QuadTree.cpp
 *
 *  Created on: 05-03-2013
 *      Author: mati
 */
#include <map>

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
	for (auto iter = bodies.begin(); iter != bodies.end(); ++iter) {
		if (!(*iter)) {
			iter = bodies.erase(iter) - 1;
			continue;
		}
		if ((*iter)->destroyed) {
			if ((*iter)->dynamically_allocated) {
				delete *iter;
			}
			iter = bodies.erase(iter) - 1;
		} else if (!IS_SET((*iter)->state, Body::STATIC)) {
			Rect<float> _rect((*iter)->x, (*iter)->y, (*iter)->w, (*iter)->h);
			//
			if (!rect.contains(_rect)) {
				if (parent) {
					parent->insertToSubQuad(*iter, true);
				} else {
					insertToSubQuad(*iter, false);
				}
				iter = bodies.erase(iter) - 1;
			}
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
			if (body->dynamically_allocated) {
				delete body;
			}
		} else {
			insertToSubQuad(*iter++, false);
		}
	}
}

void QuadTree::insert(Body* body) {
	insertToSubQuad(body, false);
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
		//
		if (!IS_SET(body->state, Body::STATIC)
				&& (body->y + body->h >= _bounds.y + _bounds.h
						|| body->x + body->w >= _bounds.x + _bounds.w
						|| body->x <= _bounds.x || body->y <= _bounds.y)) {
			continue;
		}
		if (_bounds.intersect(
				Rect<float>(body->x, body->y, body->w, body->h))) {
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

void QuadTree::clear() {
	bodies.clear();
	if (NW) {
		NW->clear();
		NE->clear();
		SW->clear();
		SE->clear();
	}
}

QuadTree::~QuadTree() {
	if (NW) {
		delete NW;
		delete NE;
		delete SW;
		delete SE;
	}
}

