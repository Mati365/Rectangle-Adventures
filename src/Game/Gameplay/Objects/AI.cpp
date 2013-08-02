/*
 * AI.cpp
 *
 *  Created on: 22-05-2013
 *      Author: mateusz
 */
#include "Objects.hpp"

//------------------------ AI Ślimaka

SnailAI::SnailAI(Character* _character, float _speed) :
				AI(_character),
				speed(_speed) {
}

/**
 * Sterowanie
 */
void SnailAI::drive() {
	if (!character || !character->collisions[pEngine::DOWN - 1]) {
		return;
	}
	character->velocity.x = speed;
}

/**
 * Pobieranie kolizji!
 */
void SnailAI::getCollision(pEngine* physics, usint dir, Body* body) {
	if (!character) {
		return;
	}
	if (((dir == pEngine::LEFT && character->velocity.x > 0)
			|| (dir == pEngine::RIGHT && character->velocity.x < 0))) {
		speed = -speed;
	}
}
