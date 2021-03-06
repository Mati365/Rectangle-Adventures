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
 * Pojedyncza czasteczka particle,
 * deklarowana statycznie bo szybciej!
 */
class Particle {
	public:
		Vector<float> pos;
		Vector<float> velocity;

		_Timer life;

		float angle;
		float size;

		Color col;
		Color source_col;

		Particle(const Vector<float>& _pos, float _size, usint _life_duration,
				const Color& _col) :
						pos(_pos),
						life(_life_duration),
						angle(0),
						size(_size),
						col(_col),
						source_col(_col) {
		}
};

/**
 * Abstrakcyjna klasa emitera!
 * Nie jest obiektem zas moze miec na
 * niego focusa!
 */
class ParticleEmitter: public Body {
	protected:
		Vector<float> pos_in_body;

		Rect<float>* focus;
		deque<Particle> particles;

		usint delay; // odstepy miedzy wypuszczeniem czastki
		usint timer;

		usint max_emit_count; // ilosc emitowan do usuniecia emitera
		usint emit_count;

	public:
		ParticleEmitter(const Rect<float>&, usint);

		virtual void drawObject(Window*);

		/** Pozycja emitera wzgledem obiektu */
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

/** Emitter sniegu */
class SnowEmitter: public ParticleEmitter {
	public:
		SnowEmitter(const Rect<float>&);

	protected:
		virtual void updateSystem() {
		}
		
		virtual bool drawParticle(usint, Window*);
		virtual void createNewParticle(Window*);
};

/** Emitter ognia */
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

/** Emitter jetu */
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

/** Emitter fajerwerkow */
class FireworksEmitter: public ParticleEmitter {
	private:
		pEngine* physics;

	public:
		FireworksEmitter(const Rect<float>&, usint, pEngine*);

	protected:
		virtual void updateSystem() {
		}
		
		virtual bool drawParticle(usint, Window*);
		virtual void createNewParticle(Window*);
};

#endif /* PARTICLE_HPP_ */
