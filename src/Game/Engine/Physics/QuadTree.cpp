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

QuadTree::QuadTree(const Rect<float>& _rect, usint _level, usint _max_level) :
		rect(_rect),
		level(_level),
		NW(NULL),
		NE(NULL),
		SW(NULL),
		SE(NULL) {
	if (_level + 1 > _max_level) {
		return;
	}
	NW = new QuadTree(Rect<float>(_rect.x, _rect.y, _rect.w / 2, _rect.h / 2),
			level + 1, _max_level);
	NE = new QuadTree(
			Rect<float>(_rect.x + _rect.w / 2, _rect.y, _rect.w / 2,
					_rect.h / 2), level + 1, _max_level);
	SW = new QuadTree(
			Rect<float>(_rect.x, _rect.y + _rect.h / 2, _rect.w / 2,
					_rect.h / 2), level + 1, _max_level);
	SE = new QuadTree(
			Rect<float>(_rect.x + _rect.w / 2, _rect.y + _rect.h / 2,
					_rect.w / 2, _rect.h / 2), level + 1, _max_level);
}

/**
 * Rysowanie siatki
 */
void QuadTree::drawObject(Window*) {
	if (!NW && bodies.empty()) {
		return;
	}
	oglWrapper::drawRect(rect.x, rect.y, rect.w, rect.h,
			Color(level * 10, level * 10, level * 10, 255),
			(MAX_LAYER - level) * 2);
	if (NW) {
		NW->drawObject(NULL);
		NE->drawObject(NULL);
		SW->drawObject(NULL);
		SE->drawObject(NULL);
	}
}

/**
 * Aktualizacja!
 */
void QuadTree::update() {

}

/**
 * Rozdzielenie obiektów!
 */
void QuadTree::insertToSubQuad(Body* body) {
	if (!NW) {
		bodies.push_back(body);
		return;
	}
	if (containsObject(&NW->rect, body)) {
		NW->insertToSubQuad(body);
	}
	if (containsObject(&NE->rect, body)) {
		NE->insertToSubQuad(body);
	}
	if (containsObject(&SE->rect, body)) {
		SE->insertToSubQuad(body);
	}
	if (containsObject(&SW->rect, body)) {
		SW->insertToSubQuad(body);
	}
}

void QuadTree::insert(deque<Body*>* bodies) {
	for (usint i = 0; i < bodies->size(); ++i) {
		insert((*bodies)[i]);
	}
}

void QuadTree::insert(Body* body) {
	insertToSubQuad(body);
}

void QuadTree::getBodiesAt(const Rect<float>& _rect, deque<Body*>* _bodies) {
	if (!NW) {
		_bodies = &bodies;
		return;
	}
	if (containsObject(&NW->rect, &_rect)) {
		NW->getBodiesAt(_rect, _bodies);
	} else if (containsObject(&NE->rect, &_rect)) {
		NE->getBodiesAt(_rect, _bodies);
	} else if (containsObject(&SW->rect, &_rect)) {
		SW->getBodiesAt(_rect, _bodies);
	} else if (containsObject(&SE->rect, &_rect)) {
		SE->getBodiesAt(_rect, _bodies);
	}
}

void QuadTree::getLowestElements(deque<deque<Body*> >& _bodies) {
	if (!bodies.empty()) {
		_bodies.push_back(bodies);
	}
	if (NW) {
		NW->getLowestElements(_bodies);
		NE->getLowestElements(_bodies);
		SW->getLowestElements(_bodies);
		SE->getLowestElements(_bodies);
	}
}

bool QuadTree::containsObject(const Rect<float>* quad,
	const Rect<float>* body) {
	if (body->x - OBJECT_MARGIN <= quad->x + quad->w
			&& body->x + body->w + OBJECT_MARGIN >= quad->x
			&& body->y - OBJECT_MARGIN <= quad->y + quad->h
			&& body->y + body->h + OBJECT_MARGIN >= quad->y) {
		return true;
	}
	return false;
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

