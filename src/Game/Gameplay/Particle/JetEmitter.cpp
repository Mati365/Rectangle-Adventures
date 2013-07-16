/*
 * JetEmitter.cpp
 *
 *  Created on: 03-07-2013
 *      Author: mateusz
 */
#include <cmath>

#include "Particle.hpp"

JetEmitter::JetEmitter(const Rect<float>& _pos, usint _delay, const Color& _col) :
		ParticleEmitter(_pos, _delay),
		col(_col) {
}

void JetEmitter::drawParticle(usint _index, Window* _window) {
	Particle& particle = particles[_index];

	particle.life_duration++;

	particle.pos += particle.velocity;
	particle.size *= 0.98f;
	if (particle.size <= 0.02f) {
		destroyed = true;
	}

	particle.pos.x = pos.x + (pos.w - particle.size) / 2.f;

	oglWrapper::drawRect(
			particle.pos.x, particle.pos.y, particle.size,
			pos.h * 0.1f * (particle.size / pos.w), particle.col, 2);

	if (particle.life_duration > particle.max_life_duration || destroyed) {
		particles.erase(particles.begin() + _index);
	}
}

void JetEmitter::createNewParticle(Window* _window) {
	Particle part(pos, pos.w, 30, col);

	part.pos.x = pos.x + pos_in_body.x;
	part.pos.y = pos.y + pos_in_body.y;
	part.velocity.y = 1.f;

	particles.push_back(part);
}
