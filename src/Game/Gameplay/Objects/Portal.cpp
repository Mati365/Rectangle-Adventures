/*
 * Portal.cpp
 *
 *  Created on: 09-09-2013
 *      Author: mateusz
 */
#include "Objects.hpp"

#include "../Gameplay.hpp"

using namespace Gameplay;

Portal::Portal(float _x, float _y, usint _orientation) :
				Body(_x, _y, 32, 8),
				linked(nullptr),
				teleport_procent(0.f) {
	body_inside = {nullptr, PortalBody::BODY_BEGIN};
	orientation = _orientation;
	/** BACKGROUND nie obsługuje callbacków kolizji */
	state = Body::FLYING;
	type = Body::PORTAL;
}

/** Rysowanie */
void Portal::drawObject(Window*) {
	oglWrapper::drawRect(x, y, w, h, oglWrapper::GREEN, 1);
	
	Body* _body = body_inside.body;
	if (!_body) {
		return;
	}
	Vector<float> body_pos = Camera::getFor(nullptr).getFocusScreenPos();
	
	// Odświęzanie obiektu wewnątrz
	updateBodyInside();
	
	// Scyzoryk!!
	glEnable(GL_SCISSOR_TEST);
	glScissor(body_pos.x, 480 -body_pos.y - _body->h, _body->w, _body->h / 2);

	_body->drawObject(nullptr);
	
	glDisable(GL_SCISSOR_TEST);
}

/** Odświeżanie */
void Portal::updateBodyInside() {
	switch (orientation) {
		case pEngine::UP:
			break;
	}
	teleport_procent++;
}

/** Wchodzenie do portalu */
bool Portal::enter(Body* body, usint _dir) {
	if (body_inside.body || !linked || !body
			|| invertDir(_dir) != orientation) {
		return false;
	}
	
	// Dołączanie obiektów do portali
	body_inside = {body, PortalBody::BODY_BEGIN};
	linked->body_inside = {body, PortalBody::BODY_END};

	// Wyłączenie renderingu i fyzki!
	body_inside.body->setState(Body::FLYING | Body::HIDDEN);
	
	// Centrowanie obiektu na środek portalu
	body->x = x + w / 2 - body->w / 2;
	
	//
	return true;
}
