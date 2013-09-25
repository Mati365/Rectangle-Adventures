/*
 * Body.cpp
 *
 *  Created on: 22-08-2013
 *      Author: mateusz
 */
#include "Physics.hpp"

#include "../../Gameplay/Objects/Objects.hpp"
#include "../../Gameplay/Objects/Weapons.hpp"

using namespace Physics;

/** Konstruktory */
Body::Body() :
				state(NONE),
				roughness(DEFAULT_ROUGHNESS),
				weight(0),
				type(PLATFORM),
				factory_type(0),
				layer(STATIC_LAYER),
				script_id(0),
				orientation(pEngine::UP),
				life_timer(0),
				physics(nullptr) {
	x = 0;
	y = 0;
	w = 0;
	h = 0;
}

Body::Body(float _x, float _y, float _w, float _h, float _roughness,
		float _weight, usint _state) :
				state(_state),
				roughness(_roughness),
				weight(_weight),
				type(PLATFORM),
				factory_type(0),
				layer(STATIC_LAYER),
				script_id(0),
				orientation(pEngine::UP),
				life_timer(0),
				physics(nullptr) {
	x = _x;
	y = _y;
	w = _w;
	h = _h;
}

/** Dlugosc zycia obiektu po czym zostanie usuniety */
void Body::setMaxLifetime(usint _max_lifetime) {
	life_timer.max_cycles_count = _max_lifetime;
	life_timer.reset();
}

/** Wymiary obiektu */
void Body::setBounds(float _x, float _y, float _w, float _h) {
	x = _x;
	y = _y;
	w = _w;
	h = _h;
}
