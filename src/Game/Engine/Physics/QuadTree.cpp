/*
 * QuadTree.cpp
 *
 *  Created on: 05-03-2013
 *      Author: mati
 */
#include <map>

#include "Physics.hpp"

using namespace std;
using namespace Physics;

#define OBJECT_MARGIN 5

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
 * Rysowanie siatki
 */
void QuadTree::drawObject(Window*) {
	if (!NW && bodies.empty()) {
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
bool QuadTree::insertToSubQuad(Body* body) {
	if (!rect.intersect(*body)) {
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
	if (NW->insertToSubQuad(body) || NE->insertToSubQuad(body)
			|| SE->insertToSubQuad(body) || SW->insertToSubQuad(body)) {
		return true;
	}
	return false;
}

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
			insertToSubQuad(*iter++);
		}
	}
}

void QuadTree::insert(Body* body) {
	insertToSubQuad(body);
}

/**
 * Pobieranie elemntów z wycinka!
 */
void QuadTree::getBodiesAt(Rect<float>& _bounds, deque<Body*>& _bodies) {
	if (!_bounds.intersect(rect)) {
		return;
	}
	for (usint i = 0; i < bodies.size(); ++i) {
		Body* body = bodies[i];
		Rect<float> rect(body->x, body->y, body->w, body->h);

		bool _intersect = _bounds.intersect(rect);
		bool _contains = _bounds.contains(rect);
		/**
		 * Sprawdzenie czy obiekt buforowany
		 */
		if(!_contains && _intersect) {
			ADD_FLAG(body->state, Body::BUFFERED);
		} else {
			UNFLAG(body->state, Body::BUFFERED);
		}
		if (_intersect || _contains) {
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

