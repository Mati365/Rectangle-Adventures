/*
 * Weapons.hpp
 *
 *  Created on: 26-06-2013
 *      Author: mateusz
 */

#ifndef WEAPONS_HPP_
#define WEAPONS_HPP_
#include "../../Engine/Graphics/Engine.hpp"
#include "../../Engine/Physics/Physics.hpp"

#include "Objects.hpp"

using namespace Engine;
using namespace Physics;

/**
 * Kule to tak naprawde przeciwnicy!
 */
class Bullet: public Character {
#define BULLET_WIDTH 8
#define BULLET_HEIGHT 8
	private:
		/**
		 * Wysokość lotu
		 */
		float max_flight_height;
		float flight_height;

	public:
		Bullet(float _x, float _y, PlatformShape* _shape,
				usint _max_flight_height) :
				Character("", _x, _y, _shape, Character::BULLET),
				//
				max_flight_height(_max_flight_height),
				flight_height(0) {
			dynamically_allocated = true;
		}

		virtual void catchCollision(pEngine*, usint, Body*);
		virtual void drawObject(Window*);
};

/**
 * Emitery!
 */
class Gun: public IrregularPlatform {
	protected:
		PlatformShape* bullet_shape;

		// Odstęp między wystrzałem
		usint shot_delay;
		usint actual_delay;

		// Fizyka
		pEngine* physics;

	public:
		Gun(pEngine*, float, float, float, PlatformShape*, PlatformShape*,
			usint);

		virtual void drawObject(Window*);

	private:
		// Wystrzał
		void shot();
};

#endif /* WEAPONS_HPP_ */
