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

/** Kule to naprawde przeciwnicy */
class Bullet: public Character {
#define BULLET_WIDTH 8
#define BULLET_HEIGHT 12
		
	private:
		/** Dystans lotu */
		float max_flight_distance;
		float flight_distance;

		Vector<float> direction;
		Vector<float> start_pos; // pozycja startowa bo grawitacja moze zniesc
		
	public:
		Bullet(float, float, const Vector<float>&, PlatformShape*, usint, usint,
				const CharacterStatus&);

		virtual void catchCollision(pEngine*, usint, Body*);
		virtual void drawObject(Window*);

		virtual ~Bullet() {
		}
};

/** Bron */
class Gun: public IrregularPlatform {
	protected:
		PlatformShape* bullet_shapes[4];

		/** Odstep miedzy wystrzalem */
		_Timer shot_delay;

		/** Fizyka */
		pEngine* physics;

		/** Dzwiek wystrzalu */
		sf::Sound* shot_sound;

	public:
		Gun(pEngine*, float, float, PlatformShape*,
				initializer_list<PlatformShape*>, usint);

		virtual void drawObject(Window*);

		/** Ustawienie tekstur */
		void setBulletsShape(initializer_list<PlatformShape*> _bullet_shapes) {
			uninitialized_copy(
					_bullet_shapes.begin(),
					_bullet_shapes.end(),
					bullet_shapes);
		}
		
		virtual ~Gun();
		
	private:
		/** Wystrzal */
		void shot();
};

#endif /* WEAPONS_HPP_ */
