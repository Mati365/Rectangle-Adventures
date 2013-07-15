/*
 * ParticleEmitter.cpp
 *
 *  Created on: 14-04-2013
 *      Author: mateusz
 */
#include "Particle.hpp"

ParticleEmitter::ParticleEmitter(const Rect<float>& _pos, usint _delay) :
		pos(_pos),
		focus(NULL),
		delay(_delay),
		timer(0),
		max_emit_count(0),
		emit_count(0) {
}

/**
 * Maksymalna ilość cykli emitowania!
 */
void ParticleEmitter::setEmitCount(usint _emit_count) {
	max_emit_count = _emit_count;
}

/**
 * Rysowanie wszystkich cząstek!
 */
void ParticleEmitter::drawObject(Window* _window) {
	if (max_emit_count != 0) {
		emit_count++;
		if (emit_count > max_emit_count) {
			destroyed = true;
			emit_count = max_emit_count;
			return;
		}
	}
	timer++;
	if (timer > delay) {
		createNewParticle(_window);
		timer = 0;
	}
	if (focus) {
		pos = *focus;
	}
	updateSystem();
	for (usint i = 0; i < particles.size();) {
		usint size = particles.size();
		drawParticle(i, _window);
		if (size == particles.size()) {
			++i;
		}
	}
}
