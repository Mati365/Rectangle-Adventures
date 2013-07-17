/*
 * FireEmitter.cpp
 *
 *  Created on: 11-05-2013
 *      Author: mateusz
 */
#include "Particle.hpp"
#include "../../Tools/Tools.hpp"

FireEmitter::FireEmitter(const Rect<float>& _pos, usint _delay) :
		ParticleEmitter(_pos, _delay) {
}

/**
 * Rysowanie pojedynczej cząsteczki..
 */
void FireEmitter::drawParticle(usint _index, Window* _window) {
	Particle& particle = particles[_index];

	particle.life_duration++;
	particle.pos += particle.velocity;
	particle.size *= (1 - particle.life_duration / particle.max_life_duration);
	if (particle.col.g > 30) {
		particle.col.g = 255
				- 255 * particle.life_duration / particle.max_life_duration;
		particle.col.a = particle.col.g;
	} else {
		destroyed = true;
	}
	oglWrapper::drawRect(particle.pos.x, particle.pos.y, particle.size,
			particle.size, particle.col, 1);

	if (particle.life_duration > particle.max_life_duration || destroyed) {
		particles.erase(particles.begin() + _index);
	}
}

/**
 * Tworzenie cząsteczki..
 */
void FireEmitter::createNewParticle(Window* _window) {
	for (usint i = 1; i < getIntRandom(6, 30); ++i) {
		Particle part(pos, getIntRandom(10, 20), getIntRandom(10, 80),
				Color(255, 255, 0));

		int rx = getIntRandom<int>(-pos.w / 2, pos.w / 2);
		int ry = getIntRandom<int>(-pos.w / 2, pos.w / 2);
		float proc = (float) abs(rx) / ((float) pos.w / 2.f);

		part.pos.x = pos.x + rx;
		part.pos.y = pos.y + ry;
		if (proc > 0.9) {
			part.col.r = 128;
			part.col.g = 128;
			part.col.b = 128;
		}

		float v_y = -0.5 * (1 - proc);
		if (v_y == 0) {
			v_y = -0.5;
		}

		part.velocity.y = v_y;
		part.max_life_duration = pos.h / (v_y < 0 ?
				-v_y : v_y) * (1 - proc) + 1;

		particles.push_back(part);
	}
}

