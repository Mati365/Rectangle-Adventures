/*
 * ParticleEmitter.cpp
 *
 *  Created on: 14-04-2013
 *      Author: mateusz
 */
#include "Particle.hpp"

ParticleEmitter::ParticleEmitter(const Rect<float>& _pos, usint _delay) :
		focus(NULL),
		delay(_delay),
		timer(0),
		max_emit_count(0),
		emit_count(0) {
	x = _pos.x;
	y = _pos.y;
	w = _pos.w;
	h = _pos.h;
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
		x = focus->x;
		y = focus->y;
	}
	updateSystem();
	//
	glLineWidth(1);
	for (usint i = 0; i < particles.size();) {
		if (drawParticle(i, _window)) {
			++i;
		}
	}
}
