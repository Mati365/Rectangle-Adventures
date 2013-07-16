/*
 * Platform.cpp
 *
 *  Created on: 12-03-2013
 *      Author: mati
 */
#include <cmath>
#include <string>

#include "Objects.hpp"

#include "../../Tools/Logger.hpp"

Platform::Platform(float _x, float _y, float _w, float _h, const Color& _col,
	usint _state) :
		Body(_x, _y, _w, _h),
		col(_col),
		repeat_movement(true),
		type(SIMPLE) {
	state = _state;
	elasticity = 0.45f;
	flag = PLATFORM;
	//
	for (usint i = 0; i < 4; ++i) {
		border[i] = false;
	}
}

void Platform::updatePlatform() {
	if (max_distance.x != 0 || max_distance.y != 0) {
		{
			float x = abs(distance.x), y = abs(distance.y), to_x = abs(
					max_distance.x), to_y = abs(max_distance.y);
			if ((x > 0 && x >= to_x) || (y > 0 && y >= to_y)) {
				if (!repeat_movement) {
					velocity.x = velocity.y = 0;
					return;
				} else {
					distance.x = distance.y = 0;
					velocity.x = -velocity.x;
					velocity.y = -velocity.y;
				}
			}
		}
		x += velocity.x;
		y += velocity.y;
		//
		distance.x += distance.x < max_distance.x ?
				velocity.x : -velocity.x;
		distance.y += distance.y < max_distance.y ?
				velocity.y : -velocity.y;
		//
	}
}

void Platform::setMovingDir(const Vector<float>& _velocity,
	const Vector<float>& _distance, bool _repeat_movement) {
	velocity = _velocity;
	max_distance = _distance;
	repeat_movement = _repeat_movement;
	distance.x = distance.y = 0;
}

/**
 * Ustawienie obramowania!
 */
void Platform::setBorder(bool top, bool right, bool down, bool left) {
	border[pEngine::UP] = top;
	border[pEngine::RIGHT] = right;
	border[pEngine::DOWN] = down;
	border[pEngine::LEFT] = left;
}

void Platform::drawBorder() {
	/**
	 * Obramowanie!
	 */
	glColor4ub(col.r, col.g, col.b, col.a);
	glLineWidth(3);
	if (border[0] && border[1] && border[2] && border[3]) {
		glBegin (GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glVertex2f(x, y);
		glEnd();
	} else {
		glBegin (GL_LINES);
		if (border[pEngine::UP]) {
			glVertex2f(x, y);
			glVertex2f(x + w, y);
		}
		if (border[pEngine::DOWN]) {
			glVertex2f(x, y + h);
			glVertex2f(x + w, y + h);
		}
		if (border[pEngine::LEFT]) {
			glVertex2f(x, y);
			glVertex2f(x, y + h);
		}
		if (border[pEngine::RIGHT]) {
			glVertex2f(x + w, y);
			glVertex2f(x + w, y + h);
		}
		glEnd();
	}
}

void Platform::drawObject(Window*) {
	/**
	 * oglWrapper nie wykorzystywany!
	 * Mała wydajność!
	 *
	 */
	updatePlatform();
	drawBorder();

	if (!IS_SET(state, Body::STATIC))
		return;
	/**
	 * Wypełnienie!
	 */
	glColor4ub(col.r, col.g, col.b, col.a * 0.5f);
	glLineWidth(1);

	switch (type) {
		/**
		 *  Proste!
		 */
		case SIMPLE:
			glBegin (GL_LINES);
			if (w > h) {
				for (usint i = 0; i < w / 10; ++i) {
					glVertex2f(x + i * 10, y + h);
					glVertex2f(x + i * 10, y);
				}
			} else {
				for (usint i = 0; i < h / 10; ++i) {
					glVertex2f(x + w, y + i * 10);
					glVertex2f(x, y + i * 10);
				}
			}
			glEnd();
			break;
			/**
			 *  Ukośne!
			 */
		case DIAGONAL:
			glBegin(GL_LINES);
			if (w > h) {
				for (usint i = 0; i < w / 10; ++i) {
					glVertex2f(x + i * 10, y + h);
					glVertex2f(x + i * 10 + 10, y);
				}
			} else {
				for (usint i = 0; i < h / 10; ++i) {
					glVertex2f(x + w, y + i * 10);
					glVertex2f(x, y + i * 10 + 10);
				}
			}
			glEnd();
			break;
			/**
			 *  Alpha w środku!
			 */
		case FILLED:
			oglWrapper::drawFillRect(x, y, w, h, col);
			break;
	}
}
//---------------------------------------

IrregularPlatform::IrregularPlatform(float _x, float _y, usint _state,
	PlatformShape* _shape) :
		Platform(_x, _y, 0, 0, oglWrapper::WHITE, _state),
		scale(1) {
	if (!_shape) {
		logEvent(Logger::LOG_ERROR, "Nie mogę załadować pustej tekstury!");
		return;
	}
	setShape(_shape);
}

void IrregularPlatform::setShape(PlatformShape* _shape) {
	const Vector<float>& _bounds = _shape->getBounds();
	//
	w = _bounds.x;
	h = _bounds.y;
	shape = _shape;
}

/**
 * Dostosuj proporcje do szerokości!
 */
void IrregularPlatform::fitToWidth(float _w) {
	setScale(_w / w);
}

void IrregularPlatform::drawObject(Window*) {
	updatePlatform();
	//
	glPushMatrix();
	glTranslatef(x, y, 1);
	if (scale != 1.f) {
		glScalef(scale, scale, 1.f);
	}
	if (shape) {
		glColor4ub(col.r, col.g, col.b, col.a);
		glCallList(shape->getID());
	}
	glPopMatrix();
	//oglWrapper::drawRect(x, y, w, h, oglWrapper::GREEN, 1);
}
