/*
 * Camera.cpp
 *
 *  Created on: 11-09-2013
 *      Author: mateusz
 */
#include "Gameplay.hpp"

using namespace Gameplay;

Camera::Camera(Body* _focus) :
				focus(_focus),
				scrolling(false) {
	pos.w = screen_bounds.x;
	pos.h = screen_bounds.y;
}

/** Odświeżanie pocycji kamery */
void Camera::updateCam(Window* _window) {
	if (!focus) {
		return;
	}
	Vector<float> target_pos(
			focus->x - pos.w / 2 + focus->w / 2 + focus->velocity.x,
			focus->y - pos.h / 2 + focus->h / 2 - Y_SPACE + focus->velocity.y);

	if (scrolling) {
		float scroll_speed = .75f;

		/** Klasyczny sposób scrollingu :P */
		if (pos.x < target_pos.x) {
			pos.x += scroll_speed;
		} else if (pos.x > target_pos.x) {
			pos.x -= scroll_speed;
		}

		if (pos.y < target_pos.y) {
			pos.y += scroll_speed;
		} else if (pos.y > target_pos.y) {
			pos.y -= scroll_speed;
		}

		/** A jeśli na miejscu? */
		if (pos.x >= target_pos.x - scroll_speed
				&& pos.x <= target_pos.x + scroll_speed
				&& pos.y >= target_pos.y - scroll_speed
				&& pos.y <= target_pos.y + scroll_speed) {
			cout << "S" << endl;
			scrolling = false;
		}
	} else {
		pos.getFromVec(target_pos);
	}
}

/** Pobieranie pozycji względem krawędzi okna */
Vector<float> Camera::getFocusScreenPos() const {
	return Vector<float>(
			focus->x + focus->w / 2 - pos.x,
			focus->y + focus->h / 2 - pos.y - Y_SPACE);
}

/** Skrollowanie do.. */
void Camera::scrollTo(Body* _focus) {
	if (_focus) {
		focus = _focus;
	}
	scrolling = true;
}

/** Ustawienie focusa */
void Camera::setFocus(Body* _focus) {
	focus = _focus;
}
