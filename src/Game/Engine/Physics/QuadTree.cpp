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
	NW = new QuadTree(this, Rect<float>(rect.x, rect.y, rect.w / 2, rect.h / 2),
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
	        Rect<float>(rect.x + rect.w / 2, rect.y + rect.h / 2, rect.w / 2,
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
	oglWrapper::drawRect(rect.x, rect.y, rect.w, rect.h,
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
 * Rozdzielenie obiektów!
 */
void QuadTree::remove(Body* body) {
	if (!NW) {
		for (usint i = 0; i < bodies.size(); ++i) {
			if (body == bodies[i]) {
				bodies.erase(bodies.begin() + i);
				return;
			}
		}
		return;
	}
	if (NW->rect.intersect(*body)) {
		NW->remove(body);
	} else if (NE->rect.intersect(*body)) {
		NE->remove(body);
	} else if (SE->rect.intersect(*body)) {
		SE->remove(body);
	} else if (SW->rect.intersect(*body)) {
		SW->remove(body);
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
	for (usint i = 0; i < bodies->size(); ++i) {
		insertToSubQuad((*bodies)[i]);
	}
}

void QuadTree::insert(Body* body) {
	insertToSubQuad(body);
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

