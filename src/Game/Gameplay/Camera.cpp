/*
 * Camera.cpp
 *
 *  Created on: 11-09-2013
 *      Author: mateusz
 */
#include "Gameplay.hpp"

using namespace Gameplay;

Camera::Camera(Body* _focus) :
				scrolling(true),
				look_timer(240) {
	pos.w = screen_bounds.x;
	pos.h = screen_bounds.y;
	
	focus.push_back(_focus);
	
	look_timer.active = false;
}

/** Odswiezanie pozycji kamery */
void Camera::updateCam(Window* _window) {
	if (focus.empty()) {
		return;
	}
	
	/** Sprawdzenie kolejki focusow */
	Body* focus = nullptr;
	
	look_timer.tick();
	if (!look_timer.active && this->focus.size() > 1) {
		this->focus.pop_back();
		look_timer.reset();
	}
	focus = getFocus();
	
	/** Kierowanie sie do focusa */
	Vector<float> target_pos(
			focus->x - pos.w / 2 + focus->w / 2 + focus->velocity.x,
			focus->y - pos.h / 2 + focus->h / 2 - Y_SPACE + focus->velocity.y);
	
	/**
	 * Jesli wlaczony look_timer to nie ma
	 * naglego przeskoku kamery na nowy focus
	 */
	if (scrolling || look_timer.active) {
		Vector<float> point(target_pos.x - pos.x, target_pos.y - pos.y);
		float c = sqrt(point.x * point.x + point.y * point.y);
		float scroll_speed = c * 20.f / ((pos.w + pos.h) / 4);
		
		if (pos.x < target_pos.x - scroll_speed) {
			pos.x += scroll_speed;
		} else if (pos.x > target_pos.x + scroll_speed) {
			pos.x -= scroll_speed;
		}
		
		if (pos.y < target_pos.y - scroll_speed) {
			pos.y += scroll_speed;
		} else if (pos.y > target_pos.y + scroll_speed) {
			pos.y -= scroll_speed;
		}
	} else {
		pos.getFromVec(target_pos);
	}
}

/** Pobieranie pozycji wzgledem krawedzi okna */
Vector<float> Camera::getFocusScreenPos() {
	Body* focus = getFocus();
	
	return Vector<float>(
			focus->x + focus->w / 2 - pos.x,
			focus->y - focus->h / 2 - pos.y - Y_SPACE);
}

/** Popatz na */
void Camera::lookAt(Body* _body) {
	if (!_body) {
		return;
	}
	focus.push_back(_body);
	look_timer.reset();
}

/** Skrollowanie do.. */
void Camera::scrollTo(Body* _focus) {
	if (_focus) {
		setFocus(_focus);
	}
	scrolling = true;
}

/** Ustawienie focusa */
void Camera::setFocus(Body* _focus) {
	focus.clear();
	focus.push_back(_focus);
}
