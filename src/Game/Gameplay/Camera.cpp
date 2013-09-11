/*
 * Camera.cpp
 *
 *  Created on: 11-09-2013
 *      Author: mateusz
 */
#include "Gameplay.hpp"

using namespace Gameplay;

Camera::Camera(Body* _focus) :
				focus(_focus) {
	pos.w = screen_bounds.x;
	pos.h = screen_bounds.y;
}

/** Odświeżanie pocycji kamery */
void Camera::updateCam(Window* _window) {
	if (!focus) {
		return;
	}
	pos.x = focus->x - pos.w / 2 + focus->w / 2 + focus->velocity.x;
	pos.y = focus->y - pos.h / 2 + focus->h / 2 - Y_SPACE + focus->velocity.y;
}

/** Pobieranie pozycji względem krawędzi okna */
Vector<float> Camera::getFocusScreenPos() const {
	return Vector<float>(
			focus->x + focus->w / 2 - pos.x,
			focus->y + focus->h / 2 - pos.y - Y_SPACE);
}

/** Ustawienie focusa */
void Camera::setFocus(Body* _focus) {
	focus = _focus;
}
