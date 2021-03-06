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

/** Rysowanie */
bool FireEmitter::drawParticle(usint _index, Window* _window) {
	Particle& particle = particles[_index];
	
	particle.life.tick();
	particle.pos += particle.velocity;
	
	float prop = (float) particle.life.cycles_count
			/ (float) particle.life.max_cycles_count;
	
	particle.size *= (1 - prop);
	
	if (particle.col.g > 10) {
		particle.col.g = 255 - 255 * prop;
		particle.col.a = particle.col.g;
	}
	oglWrapper::drawRect(
			particle.pos.x,
			particle.pos.y,
			particle.size,
			particle.size,
			particle.col,
			1);
	
	if (!particle.life.active || destroyed) {
		particles.erase(particles.begin() + _index);
		return false;
	}
	return true;
}

/** Tworzenie czastki */
void FireEmitter::createNewParticle(Window* _window) {
	for (usint i = 1; i < getIntRandom(6, 30); ++i) {
		Particle part(
				Vector<float>(x, y),
				getIntRandom(10, 20),
				getIntRandom(10, 80),
				Color(255, 255, 0));
		
		int rx = getIntRandom<int>(-w / 2, w / 2);
		int ry = getIntRandom<int>(-w / 2, w / 2);
		float proc = (float) abs(rx) / ((float) w / 2.f);
		
		part.pos.x = x + rx;
		part.pos.y = y + ry;
		if (proc > 0.9) {
			part.col.r = 128;
			part.col.g = 128;
			part.col.b = 128;
		}
		
		float v_y = -1.0 * (1 - proc);
		if (v_y == 0) {
			v_y = -1.0;
		}
		
		part.velocity.y = v_y;
		part.life.max_cycles_count = h / (v_y < 0 ? -v_y : v_y) * (1 - proc)
				+ 1;
		
		particles.push_back(part);
	}
}

