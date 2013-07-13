/*
 * SnowEmitter.cpp
 *
 *  Created on: 15-04-2013
 *      Author: mateusz
 */
#include "Particle.hpp"
#include "../../Tools/Tools.hpp"

SnowEmitter::SnowEmitter(const Rect<float>& _pos) :
		ParticleEmitter(_pos, 40) {
}

void SnowEmitter::drawParticle(usint _index, Window* _window) {
	Particle& particle = particles[_index];

	particle.life_duration++;
	particle.pos += particle.velocity;
	if (particle.col.a > 30) {
		particle.col.a = 255
				- 255 * particle.life_duration / particle.max_life_duration;
	}

	oglWrapper::drawRect(particle.pos.x, particle.pos.y, particle.size,
			particle.size, particle.col, 1);

	if (particle.life_duration > particle.max_life_duration
			|| particle.col.a < 30) {
		particles.erase(particles.begin() + _index);
	}
}

void SnowEmitter::createNewParticle(Window* _window) {
	for (usint i = 0; i < getIntRandom(2, 30); ++i) {
		Particle part(pos, getIntRandom(10, 20), getIntRandom(50, 200),
				oglWrapper::GRAY);

		part.velocity.y = getIntRandom(150, 350) / 100;
		part.pos.x = pos.x + getIntRandom(-100, (int) pos.w + 100);

		particles.push_back(part);
	}
}
