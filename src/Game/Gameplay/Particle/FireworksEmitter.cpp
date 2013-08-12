/*
 * FireworksEmitter.cpp
 *
 *  Created on: 12-08-2013
 *      Author: mateusz
 */
#include "Particle.hpp"

FireworksEmitter::FireworksEmitter(const Rect<float>& _pos, usint _delay,
		pEngine* _physics) :
				ParticleEmitter(_pos, _delay),
				physics(_physics) {
}

/**
 * Rysowanie petard
 */
bool FireworksEmitter::drawParticle(usint _index, Window* _window) {
	Particle& particle = particles[_index];

	particle.life_duration++;
	particle.pos -= particle.velocity;

	oglWrapper::drawRect(
			particle.pos.x,
			particle.pos.y,
			particle.size,
			particle.size,
			particle.col,
			2);

	if (particle.life_duration > particle.max_life_duration || destroyed) {
		generateExplosion(
				physics,
				Rect<float>(
						particle.pos.x,
						particle.pos.y,
						particle.size,
						particle.size),
				50,
				particle.col,
				particle.size / 6,
				particle.size / 3,
				Vector<float>(getIntRandom<int>(1, 3), getIntRandom<int>(1, 3)),
				25,
				Body::BACKGROUND);
		//
		particles.erase(particles.begin() + _index);
		return false;
	}
	return true;
}

/**
 * Petardy są różnego koloru
 */
void FireworksEmitter::createNewParticle(Window* _window) {
	Particle part(
			Vector<float>(x + getIntRandom<usint>(0, w), y + 111),
			getIntRandom<usint>(10, 20),
			getIntRandom<usint>(20, 50),
			Color(
					getIntRandom<usint>(28, 255),
					getIntRandom<usint>(28, 255),
					getIntRandom<usint>(28, 255)));

	part.velocity.y = (float) getIntRandom<int>(3, 10) / 5;

	particles.push_back(part);
}

