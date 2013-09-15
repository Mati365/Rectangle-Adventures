/*
 * Portal.cpp
 *
 *  Created on: 09-09-2013
 *      Author: mateusz
 */
#include "Objects.hpp"

#include "../Gameplay.hpp"

using namespace Gameplay;

Portal::Portal(float _x, float _y, usint _orientation, usint _flag) :
				Body(_x, _y, 32, 1),
				linked(nullptr),
				teleport_procent(0.f) {
	body_inside = {nullptr, _flag};
	orientation = _orientation;

	/** BACKGROUND nie obsługuje callbacków kolizji */
	type = Body::PORTAL;
	state = Body::FLYING;

	/** Jeśli pionowy to zamiana wartości */
	if(isHorizontalDir(_orientation)) {
		xor_swap<int>((int*)&w, (int*)&h);
	}
}

/** Linkowanie */
void Portal::linkTo(Portal* _linked) {
	linked = _linked;

	// Linkowanie samego siebie
	linked->linked = this;
	linked->body_inside.flag =
			body_inside.flag == PortalBody::BODY_BEGIN ?
					PortalBody::BODY_END : PortalBody::BODY_BEGIN;
}

/** Pobieranie pozycji stencil buffora */
Rect<float> Portal::getStencilTexCoord() {
	Rect<float> pos;
	Body* _body = body_inside.body;

	if (!_body) {
		return pos;
	}

	/** Uaktualnianie wymiarów */
	pos.w = _body->w;
	pos.h = _body->h;

	/** Centrowanie obiektu dla poziomych */
	if (orientation == pEngine::DOWN || orientation == pEngine::UP) {
		pos.x = x + w / 2 - _body->w / 2;
		_body->x = pos.x;
		/**
		 * Znając procent można obliczyć zagłębienie się
		 * obiektu i wynurzenie po przeciwnej stronie
		 * portalu
		 */
		switch (body_inside.flag) {
			/**
			 * Początek ciała:
			 */
			case PortalBody::BODY_BEGIN:
				if (orientation == pEngine::UP) {
					/** Obiekt grawitacją wciągany do dołu */
					pos.h = (1.f - teleport_procent)
							* body_inside.body_bounds.h;

					/** Uaktualnianie pozycji */
					pos.y = y - pos.h;
					_body->y = pos.y;
				} else {
					/** Wciąganie obiektu do góry */
					pos.h = body_inside.body_bounds.h;
					_body->y = y - teleport_procent * body_inside.body_bounds.h;
					pos.y = y;
				}
				break;

				/**
				 * Koniec ciała:
				 */
			case PortalBody::BODY_END:
				if (orientation == pEngine::DOWN) {
					/** Wypadanie obiektu z góry */
					_body->y = y
							- body_inside.body_bounds.h
									* (1.f - teleport_procent);
					pos.h = body_inside.body_bounds.h;
					pos.y = y;
				} else {
					/** Wysuwanie obiektu z góry */
					pos.h = body_inside.body_bounds.h
							- (1.f - teleport_procent)
									* body_inside.body_bounds.h;
					_body->y = y - teleport_procent * body_inside.body_bounds.h;
					pos.y = _body->y;
				}
				break;
		};
	}

	/** Centrowanie obiektu dla Pionowych */
	if (orientation == pEngine::LEFT || orientation == pEngine::RIGHT) {
		pos.y = y - h / 2 + _body->h / 2;
		_body->y = pos.y;
		/**
		 * Znając procent można obliczyć zagłębienie się
		 * obiektu i wynurzenie po przeciwnej stronie
		 * portalu
		 */
		switch (body_inside.flag) {
			/**
			 * Początek ciała:
			 */
			case PortalBody::BODY_BEGIN:
				if (orientation == pEngine::RIGHT) {
					/** Obiekt grawitacją wciągany w lewo */
					pos.w = body_inside.body_bounds.w;

					/** Uaktualnianie pozycji */
					pos.x = x;
					_body->x = pos.x
							- teleport_procent * body_inside.body_bounds.w;
				} else {
					/** Wciąganie w prawą stronę */
					pos.w = (1.f - teleport_procent)
							* body_inside.body_bounds.w;
					pos.x = x - pos.w;
					_body->x = pos.x;
				}
				break;

				/**
				 * Koniec ciała:
				 */
			case PortalBody::BODY_END:
				if (orientation == pEngine::LEFT) {
					/** Obiekt grawitacją wciągany w lewo */
					pos.w = body_inside.body_bounds.w
							- (1.f - teleport_procent)
									* body_inside.body_bounds.w;

					/** Uaktualnianie pozycji */
					pos.x = x - teleport_procent * body_inside.body_bounds.w;
					_body->x = pos.x;
				} else {
					/** Obiekt wyciągany w prawo */
					pos.w = body_inside.body_bounds.w;

					/** Uaktualnianie pozycji */
					pos.x = x;
					_body->x = x
							- (1.f - teleport_procent)
									* body_inside.body_bounds.w;
				}
				break;
		};
	}

	//
	return pos;
}

/** Rysowanie */
void Portal::drawObject(Window*) {
	if (!linked) {
		return;
	}
	/** Główny rendering */

	beginStroke(0xF0F0);

	glLineWidth(2.f);
	if (body_inside.flag == PortalBody::BODY_BEGIN) {
		glColor3ub(255.f, 255.f, 255.f);
	} else {
		glColor3ub(155.f, 155.f, 155.f);
	}
	glBegin(GL_LINE_STRIP);

	glVertex2f(x, y);
	glVertex2f(x, linked->y);
	glVertex2f(linked->x, linked->y);

	glEnd();

	endStroke();

	oglWrapper::drawRect(
			x,
			y,
			w,
			h,
			body_inside.flag == PortalBody::BODY_END ?
					oglWrapper::RED : oglWrapper::GREEN,
			2.f);

	/** Rysowanie portalu */
	Body* _body = body_inside.body;
	if (!_body) {
		return;
	}

	/** Odświęzanie obiektu wewnątrz */
	updateBodyInside();

	Rect<float> stencil_pos = getStencilTexCoord();
	
	/** Stencil buffer */
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glStencilFunc(GL_NEVER, 1, 0xFF);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
	glStencilMask(0xFF); // ustawienie maski tego co ma być malowane

	glClear(GL_STENCIL_BUFFER_BIT); // czyszczenie tablicy

	// Rysowanie szablonu
	glBegin(GL_QUADS);
	glVertex2f(stencil_pos.x, stencil_pos.y);
	glVertex2f(stencil_pos.x + stencil_pos.w, stencil_pos.y);
	glVertex2f(stencil_pos.x + stencil_pos.w, stencil_pos.y + stencil_pos.h);
	glVertex2f(stencil_pos.x, stencil_pos.y + stencil_pos.h);
	glEnd();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilMask(0x00);

	glStencilFunc(GL_EQUAL, 1, 0xFF);

	_body->drawObject(nullptr);

	glDisable(GL_STENCIL_TEST);
}

/** Reset */
void Portal::exitBody() {
	if (!body_inside.body) {
		return;
	}
	Body* _body = body_inside.body;

	if (body_inside.flag == PortalBody::BODY_END) {
		/** Nowa pozycja */
		Rect<float> new_pos = getStencilTexCoord();

		_body->x = new_pos.x;
		_body->y = new_pos.y;
		_body->velocity.x = _body->velocity.y = 0;

		_body->setState(Body::NONE);

		/** Odpychanie ciała */
		dodgeBody(_body, invertDir(orientation), 2);
	}

	body_inside.body = nullptr;
	teleport_procent = 0;

	if (linked) {
		linked->exitBody();
	}
}

/** Odświeżanie */
void Portal::updateBodyInside() {
	if (!linked || !body_inside.body) {
		return;
	}
	Body* _body = body_inside.body;

	if (teleport_procent < 0) {
		exitBody();
	} else if (teleport_procent < 1) {
		float speed_proc = 0.f;

		/** Wyliacznie procentu z prędkości wpadania */
		if (isVertical()) {
			speed_proc = (float) _body->velocity.y / (float) _body->h;
		} else {
			speed_proc = (float) _body->velocity.x / (float) _body->w;
		}

		/** Dodawanie procentu */
		teleport_procent += abs(speed_proc / 5);
	} else {
		/** Zerowanie */
		exitBody();
		Camera::getFor().scrollTo(nullptr);
	}
	// Synchronizacja
	linked->teleport_procent = teleport_procent;
}

/** Wchodzenie do portalu */
bool Portal::enter(Body* body, usint _dir) {
	if (_dir == orientation || teleport_procent != 0 || body_inside.body
			|| !linked || !body) {
		return false;
	}

	// Odwrócenie portalu
	if ((isVertical() && !isHorizontalDir(_dir))
			|| (isHorizontal() && isHorizontalDir(_dir))) {
		orientation = invertDir(_dir);

		body_inside.flag = PortalBody::BODY_BEGIN;
		linked->body_inside.flag = PortalBody::BODY_END;
	}

	// Dołączanie obiektów do portali
	Rect<float> body_bounds(body->x, body->y, body->w, body->h);

	body_inside = {body, PortalBody::BODY_BEGIN, body_bounds};
	linked->body_inside = {body, PortalBody::BODY_END, body_bounds};

	// Wyłączenie renderingu i fyzki!
	body_inside.body->setState(Body::FLYING | Body::HIDDEN);
	
	//
	return true;
}
