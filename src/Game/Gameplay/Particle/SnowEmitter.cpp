/*
 * SnowEmitter.cpp
 *
 *  Created on: 15-04-2013
 *      Author: mateusz
 */
#include "Particle.hpp"

#include "../../Tools/Tools.hpp"

SnowEmitter::SnowEmitter(const Rect<float>& _pos) :
				ParticleEmitter(_pos, 50) {
}

/**
 * Rysowanie pojedynczej cząsteczki..
 */
bool SnowEmitter::drawParticle(usint _index, Window* _window) {
	Particle& particle = particles[_index];
	
	particle.life.tick();
	particle.pos += particle.velocity;
	
	if (particle.col.r > 30) {
		particle.col.r = 255
				- 255 * particle.life.cycles_count
						/ particle.life.max_cycles_count;
		particle.col.g = particle.col.b = particle.col.r;
	}
	/**
	 * Optymalizacja!
	 oglWrapper::drawRect(particle.pos.x, particle.pos.y, particle.size,
	 particle.size, particle.col, 1);
	 */
	float x = particle.pos.x, y = particle.pos.y, w = particle.size;
	
	glColor3ub(particle.col.r, particle.col.g, particle.col.b);
	glBegin(GL_LINE_LOOP);
	
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + w);
	glVertex2f(x, y + w);
	
	glEnd();
	
	if (!particle.life.active || particle.col.a < 30) {
		particles.erase(particles.begin() + _index);
		return false;
	}
	return true;
}

/**
 * Tworzenie cząsteczki..
 */
void SnowEmitter::createNewParticle(Window* _window) {
	for (usint i = 0; i < getIntRandom(2, (int) (50 * (50.f / (float) delay)));
			++i) {
		float angle = getIntRandom<int>(20, 80);
		float speed = 1.f;
		
		Vector<float> wind(
				cos(TO_RAD(angle)) * speed,
				sin(TO_RAD(angle)) * speed);
		
		Particle part(
				Vector<float>(x, y),
				getIntRandom(6, 14),
				getIntRandom(50, 100),
				oglWrapper::WHITE);
		
		part.velocity = wind;
		part.angle = angle;
		
		part.velocity.y += getIntRandom(150, 350) / 150;
		part.pos.x = x + getIntRandom(-100, (int) w + 100);
		
		particles.push_back(part);
	}
}
