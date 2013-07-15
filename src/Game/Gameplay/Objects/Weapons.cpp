/*
 * Weapons.cpp
 *
 *  Created on: 26-06-2013
 *      Author: mateusz
 */
#include "Weapons.hpp"

//////////////////// Bullet

void Bullet::drawObject(Window*) {
	velocity.y = -2.5f;
	flight_height += -velocity.y;
	if (flight_height >= max_flight_height) {
		destroyed = true;
	}
	//
	Character::drawObject(NULL);
}

void Bullet::catchCollision(pEngine*, usint, Body*) {
	destroyed = true;
}

/////////////////// Gun

Gun::Gun(pEngine* _physics, float _x, float _y, float _w,
		PlatformShape* _gun_shape, PlatformShape* _bullet_shape,
		usint _shot_delay) :
		IrregularPlatform(_x, _y, State::NONE, _gun_shape),
		//
		bullet_shape(_bullet_shape),
		shot_delay(_shot_delay),
		actual_delay(0),
		physics(_physics) {
	fitToWidth(_w);
}

/**
 * Wystrzał następuje ze środka!
 */
void Gun::shot() {
	if (!physics) {
		return;
	}
	physics->insert(
			new Bullet(x + w / 2 - BULLET_WIDTH / 2, y - h * 2, bullet_shape,
					300));
}

void Gun::drawObject(Window*) {
	actual_delay++;
	if (actual_delay >= shot_delay) {
		actual_delay = 0;
		//
		shot();
	}
	IrregularPlatform::drawObject(NULL);
}

