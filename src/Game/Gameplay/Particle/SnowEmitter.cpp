/*
 * SnowEmitter.cpp
 *
 *  Created on: 15-04-2013
 *      Author: mateusz
 */
#include "Particle.hpp"

#include "../../Tools/Tools.hpp"

SnowEmitter::SnowEmitter(const Rect<float>& _pos) :
		ParticleEmitter(_pos, 120) {
}

/**
 * Rysowanie pojedynczej cząsteczki..
 */
bool SnowEmitter::drawParticle(usint _index, Window* _window) {
	Particle& particle = particles[_index];

	particle.life_duration++;
	particle.pos += particle.velocity;
	if (particle.max_life_duration == 0) {
		particles.erase(particles.begin() + _index);
		return false;
	}
	if (particle.col.a > 30) {
		particle.col.a = 255
				- 255 * particle.life_duration / particle.max_life_duration;
	}
	/**
	 * Optymalizacja!
	 oglWrapper::drawRect(particle.pos.x, particle.pos.y, particle.size,
	 particle.size, particle.col, 1);
	 */
	float x = particle.pos.x, y = particle.pos.y, w = particle.size;

	glColor4ub(particle.col.r, particle.col.g, particle.col.b, particle.col.a);
	glBegin (GL_LINE_LOOP);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + w);
	glVertex2f(x, y + w);
	glEnd();

	if (particle.life_duration > particle.max_life_duration
			|| particle.col.a < 30) {
		particles.erase(particles.begin() + _index);
		return false;
	}
	return true;
}

/**
 * Tworzenie cząsteczki..
 */
void SnowEmitter::createNewParticle(Window* _window) {
	for (usint i = 0; i < getIntRandom(2, (int) (30 * (40.f / (float) delay)));
			++i) {
		Particle part(Vector<float>(x, y), getIntRandom(10, 20),
						getIntRandom(50, 200), oglWrapper::GRAY);

		part.velocity.y = getIntRandom(150, 350) / 150;
		part.pos.x = x + getIntRandom(-100, (int) w + 100);

		particles.push_back(part);
	}
}
