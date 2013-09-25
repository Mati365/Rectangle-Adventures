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

/** Rysowanie */
bool FireworksEmitter::drawParticle(usint _index, Window* _window) {
	Particle& particle = particles[_index];
	
	particle.life.tick();
	particle.pos -= particle.velocity;
	
	oglWrapper::drawRect(
			particle.pos.x,
			particle.pos.y,
			particle.size / 4,
			particle.size,
			particle.col,
			2);
	
	if (!particle.life.active || destroyed) {
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
				45,
				Body::BACKGROUND);
		//
		particles.erase(particles.begin() + _index);
		return false;
	}
	return true;
}

/** Petardy sa roznego koloru */
void FireworksEmitter::createNewParticle(Window* _window) {
	Particle part(
			Vector<float>(x + getIntRandom<usint>(0, w), y + h),
			getIntRandom<usint>(10, 20),
			10,
			Color(
					getIntRandom<usint>(228, 255),
					getIntRandom<usint>(18, 255),
					getIntRandom<usint>(28, 255)));
	
	part.velocity.y = (float) getIntRandom<int>(5, 10) / 2;
	part.life.max_cycles_count = 60;
	
	particles.push_back(part);
}

