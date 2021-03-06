/*
 * Weapons.cpp
 *
 *  Created on: 26-06-2013
 *      Author: mateusz
 */
#include "Weapons.hpp"

#include "../../Resources/Data/SoundManager.hpp"

/** Konstruktor */
Bullet::Bullet(float _x, float _y, const Vector<float>& _direction,
		PlatformShape* _shape, usint _max_flight_distance, usint _orientation,
		const CharacterStatus& _status) :
				Character("", _x, _y, _shape, Character::BULLET),
				//
				max_flight_distance(_max_flight_distance),
				flight_distance(0),
				//
				direction(_direction),
				start_pos(_x, _y) {
	status = _status;
	orientation = _orientation;
	//
	if (orientation == pEngine::UP || orientation == pEngine::DOWN) {
		fitToWidth(BULLET_WIDTH);
	} else {
		fitToWidth(BULLET_HEIGHT);
	}
}

/** Rysowanie pocisku */
void Bullet::drawObject(Window*) {
	velocity = direction;
	/**
	 * Pocisk musi leciec prosto bo fizyka spowoduje
	 * jego opadanie
	 */
	if (orientation == pEngine::UP || orientation == pEngine::DOWN) {
		x = start_pos.x;
	} else {
		y = start_pos.y;
	}
	//
	flight_distance += abs(velocity.x == 0 ? velocity.y : velocity.x);
	if (flight_distance >= max_flight_distance) {
		destroyed = true;
	}
	//
	Character::drawObject(NULL);
}

/** Event pocisku  */
void Bullet::catchCollision(pEngine* _physics, usint _dir, Body* body) {
	if (IS_SET(body->state, Body::HIDDEN)
			|| IS_SET(body->state, Body::BACKGROUND)) {
		return;
	}
	/**
	 * Bugfix!
	 * + Nieraz przy stykaniu sie pocisku
	 * z graczem pocisk wyparowywal� zanim
	 * dal callback do gracza!
	 */
	destroyed = true;
	if (body && !body->destroyed) {
		body->catchCollision(_physics, Physics::invertDir(_dir), this);
	}
}

/** Emitter pociskow */
Gun::Gun(pEngine* _physics, float _x, float _y, PlatformShape* _gun_shape,
		initializer_list<PlatformShape*> _bullet_shapes, usint _shot_delay) :
				IrregularPlatform(_x, _y, Body::NONE, _gun_shape),
				//
				shot_delay(_shot_delay, getIntRandom<int>(0, _shot_delay)),
				physics(_physics) {
	setBulletsShape(_bullet_shapes);

	/** Rejestracja dzwieku */
	shot_sound = SoundManager::getInstance().getResourceSound(
			SoundManager::GUN_SHOT_SOUND);
}

/** Wystrzal pocisku */
void Gun::shot() {
	if (!physics) {
		return;
	}
	/** Podzial ze wzgledu na orientacje emitera */
	PlatformShape* shape = bullet_shapes[orientation - 1];
	const CharacterStatus& bullet_status =
			ResourceFactory::factory_status[ResourceFactory::BULLET].character_status;
	
	switch (orientation) {
		case pEngine::UP:
			physics->insert(
					new Bullet(
							x + w / 2 - BULLET_WIDTH / 2,
							y - h * 2,
							Vector<float>(0, -3),
							shape,
							100,
							orientation,
							bullet_status));
			break;
			
			/**
			 *
			 */
		case pEngine::DOWN:
			physics->insert(
					new Bullet(
							x + w / 2 - BULLET_WIDTH / 2,
							y + h * 2,
							Vector<float>(0, 2),
							shape,
							100,
							orientation,
							bullet_status));
			break;
			
			/**
			 *
			 */
		case pEngine::LEFT:
			physics->insert(
					new Bullet(
							x - 15,
							y - h / 2 + BULLET_HEIGHT,
							Vector<float>(-2, 0),
							shape,
							100,
							orientation,
							bullet_status));
			break;
			
			/**
			 *
			 */
		case pEngine::RIGHT:
			physics->insert(
					new Bullet(
							x + w + 15,
							y - h / 2 + BULLET_HEIGHT,
							Vector<float>(2, 0),
							shape,
							100,
							orientation,
							bullet_status));
			break;
	}
	shot_sound->Play();
}

/** Rysowanie emittera */
void Gun::drawObject(Window*) {
	shot_delay.tick();
	if (!shot_delay.active) {
		shot_delay.reset();
		//
		shot();
	}
	IrregularPlatform::drawObject(NULL);
}

/** Czyszczenie */
Gun::~Gun() {
	shot_sound->Stop();
	safe_delete<sf::Sound>(shot_sound);
}
