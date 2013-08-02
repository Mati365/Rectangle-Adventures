/*
 * Particle.hpp
 *
 *  Created on: 14-04-2013
 *      Author: mateusz
 */

#ifndef PARTICLE_HPP_
#define PARTICLE_HPP_
#include "../../Engine/Physics/Physics.hpp"
#include "../../Engine/Graphics/Engine.hpp"
#include "../Gameplay.hpp"

using namespace Gameplay;

/**
 * Pojedyncza cząsteczka particle,
 * deklarowana statycznie bo szybciej!
 */
class Particle {
	public:
		Vector<float> pos;
		Vector<float> velocity;

		usint life_duration;
		usint max_life_duration;

		float angle;
		float size;

		Color col;
		Color source_col;

		Particle(const Vector<float>& _pos, float _size,
				usint _max_life_duration, const Color& _col) :
						pos(_pos),
						life_duration(0),
						max_life_duration(_max_life_duration),
						angle(0),
						size(_size),
						col(_col),
						source_col(_col) {
		}
};

/**
 * Abstrakcyjna klasa emitera!
 * Nie jest obiektem zaś może mieć na
 * niego focusa!
 */
class ParticleEmitter: public Body {
	protected:
		Vector<float> pos_in_body;

		Rect<float>* focus;
		deque<Particle> particles;

		usint delay; // odstępy między wypuszczeniem cząstki
		usint timer;

		usint max_emit_count; // ilość emitowań po czym kasowanie!
		usint emit_count;

	public:
		ParticleEmitter(const Rect<float>&, usint);

		virtual void drawObject(Window*);

		/**
		 * Pozycja w obiekcie, na którym jest
		 * focus
		 */
		void setPosInBody(const Vector<float>& _pos) {
			pos_in_body = _pos;
		}
		void setFocus(Rect<float>* _focus) {
			focus = _focus;
		}
		
		void setEmitCount(usint);

		Rect<float>* getFocus() const {
			return focus;
		}
		
		virtual ~ParticleEmitter() {
		}
		
	protected:
		virtual void updateSystem() = 0;
		virtual bool drawParticle(usint, Window*) = 0;
		virtual void createNewParticle(Window*) = 0;
};

//--------------------------- EMITERY!

/**
 * Emiter śniegu!
 */
class SnowEmitter: public ParticleEmitter {
	public:
		SnowEmitter(const Rect<float>&);

	protected:
		virtual void updateSystem() {
		}
		
		virtual bool drawParticle(usint, Window*);
		virtual void createNewParticle(Window*);
};

/**
 * Emiter ognia!
 */
class FireEmitter: public ParticleEmitter {
	public:
		FireEmitter(const Rect<float>&, usint);

		usint getTemperature() const {
			return w * h / 60 * 100 * delay;
		}
	protected:
		virtual void updateSystem() {
		}
		
		virtual bool drawParticle(usint, Window*);
		virtual void createNewParticle(Window*);
};

/**
 * Emiter strumienia!
 */
class JetEmitter: public ParticleEmitter {
	private:
		Color col;

	public:
		JetEmitter(const Rect<float>&, usint, const Color&);

	protected:
		virtual void updateSystem() {
		}
		
		virtual bool drawParticle(usint, Window*);
		virtual void createNewParticle(Window*);
};

#endif /* PARTICLE_HPP_ */
