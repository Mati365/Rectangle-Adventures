/*
 * Characters.cpp
 *
 *  Created on: 23-03-2013
 *      Author: mati
 */
#include <GL/glew.h>

#include "Objects.hpp"

#include "../Particle/Particle.hpp"
#include "../Gameplay.hpp"

using namespace Gameplay;

//------------------------

/**
 * todo:
 * + Wczytywanie statusu!
 */
bool CharacterStatus::load(FILE* _file) {
	return false;
}

/**
 * Generowanie krwii
 */
void generateExplosion(pEngine* physics, Body* body, usint count,
		const Color& col, float min_size, float max_size) {
	float cx, cy, angle;
	/**
	 * Generowanie cząsteczek krwii w postaci eksplozji
	 */
	for (usint i = 0; i < count; ++i) {
		angle = TO_RAD(getIntRandom<int>(0.f, 360.f));
		//
		cx = body->w / 2 * cosf(angle);
		cy = body->h / 2 * sinf(angle);
		//
		float size = getIntRandom<int>(min_size, max_size);
		//
		Platform* platform = new Platform(
				body->x + cx,
				body->y + cy,
				size,
				size,
				col,
				Body::NONE);
		//
		platform->setMaxLifetime(size * 140);
		platform->setBorder(false, false, false, false);
		platform->setFillType(Platform::FILLED);
		//
		platform->layer = STATIC_LAYER + 1;
		platform->velocity = Vector<float>(cosf(angle) * 8, sinf(angle) * 12);
		//
		physics->insert(platform);
	}
}

//------------------------

Character::Character(const string& _nick, float _x, float _y,
		PlatformShape* _shape, usint _type) :
				IrregularPlatform(_x, _y, true, _shape),
				action(JUMPING),
				status(NULL, MAX_LIVES, false, 0, 0, _x, _y),
				ai(NULL),
				//
				blood_anim_visible_time(13),
				blood_anim_cycles(8) {
	type = _type;
}

/**
 * Uderzenie - zmniejszenie życia
 */
void Character::hitMe(pEngine* physics) {
	ADD_FLAG(action, BLOODING);
	//
	generateExplosion(physics, this, 5, oglWrapper::RED, 3, 6);
	//
	playResourceSound(SPIKES_SOUND);
}

/**
 * Eksplozja
 */
void Character::die(pEngine* physics, usint _dir) {
	if (isDead()) {
		return;
	}
	/**
	 * TRRUP
	 */
	status.health = DEATH;
	//
	generateExplosion(physics, this, 30, oglWrapper::RED, 3, 6);
	setShape(getShapePointer("cranium"));
	fitToWidth(14);
	//
	playResourceSound(DIE_SOUND);
}

/**
 * Kolizja gracza
 */
void Character::catchCollision(pEngine* physics, usint dir, Body* body) {
	if (isDead()) {
		return;
	}
	if (ai) {
		ai->getCollision(physics, dir, body);
	}

	/**
	 * Ginąć może nie tylko gracz
	 */
	if (dir == pEngine::DOWN && !IS_SET(body->state, Body::HIDDEN)
			&& (type == Body::HERO || type == Body::ENEMY)) {
		// Zryte formatowanie
		if (velocity.y < -9 && status.health > 0) {
			die(physics, dir);
			return;
		}
		UNFLAG(action, JUMPING);
	}
	if (type != HERO) {
		return;
	}
	
	/**
	 * Skrypty
	 */
	if (body->type == Body::TRIGGER) {
		dynamic_cast<Trigger*>(body)->generate();
		return;
	}
	
	/**
	 * Kasowanie flag
	 */
	UNFLAG(action, CLIMBING);

	/**
	 * Akcje gracza
	 */
	Character* enemy = dynamic_cast<Character*>(body);
	if (!enemy) {
		return;
	}
	switch (enemy->type) {
		/**
		 *
		 */
		case LADDER: {
			float _max_speed = physics->getGravitySpeed() * 2;
			//
			if (velocity.y > _max_speed) {
				velocity.y = _max_speed;
			} else if (velocity.y < -_max_speed * 2.f) {
				velocity.y = -_max_speed * 2.f;
			}
			if (velocity.x >= -0.5 && velocity.x <= 0.5
					&& (velocity.y == _max_speed
							|| velocity.y == -_max_speed * 2.f)) {
				x = body->x;
			}
			//
			ADD_FLAG(action, CLIMBING);
		}
			break;

			/**
			 *
			 */
		case SCORE:
			status += enemy->status;
			if (status.health > MAX_LIVES) {
				status.health = MAX_LIVES;
			}
			if (status.score > MAX_SCORE) {
				status.score = MAX_SCORE;
			}
			//
			generateExplosion(
					physics,
					body,
					6,
					*dynamic_cast<IrregularPlatform*>(body)->getShape()->getMainColor(),
					2,
					3);
			//
			body->destroyed = true;
			//
			if (enemy->status.health > 0 || enemy->status.score) {
				playResourceSound(SCORE_SOUND);
			}
			break;

			/**
			 *
			 */
		case ENEMY:
			if (dir == pEngine::DOWN || dir == pEngine::UP) {
				status += enemy->status;
				//
				body->destroyed = true;
			} else {
				status -= enemy->status;
				hitMe(physics);
			}
			dodge(dir);
			break;

			/**
			 *
			 */
		case BULLET:
			if (status.health > 0) {
				status.health -= 1;
			}
			hitMe(physics);
			dodge(dir);
			//
			body->destroyed = true;
			break;

			/**
			 *
			 */
		case SPIKES:
			if (body->orientation != invertDir(dir)) {
				break;
			}
			if (status.health > 0) {
				status.health -= 1;
			}
			hitMe(physics);
			dodge(dir);
			break;

			/**
			 *
			 */
		default:
			break;
	}
	if (status.health == 0) {
		die(physics, dir);
	}
}

/**
 * Unik
 */
void Character::dodge(usint _dir) {
	float _speed = 8;
	//
	switch (_dir) {
		case pEngine::RIGHT:
			velocity.x = -_speed;
			break;

			//
		case pEngine::LEFT:
			velocity.x = _speed;
			break;

			//
		case pEngine::UP:
			velocity.y = -_speed;
			break;

			//
		case pEngine::DOWN:
			velocity.y = _speed;
			break;
	}
	//
	x += velocity.x;
	y += velocity.y;
}

/**
 * Skok z określoną prędkością!
 */
void Character::jump(float _y_speed, bool _force) {
	if (!isDead() && (!IS_SET(action, JUMPING) || _force)) {
		ADD_FLAG(action, JUMPING);
		velocity.y = -_y_speed;
		//
		if (!_force && !IS_SET(action, CLIMBING)) {
			playResourceSound(JUMP_SOUND);
		}
	}
}

/**
 * Poruszanie się
 */
void Character::move(float x_speed, float y_speed) {
	if ((x_speed > 0 && velocity.x < 0) || (x_speed < 0 && velocity.x > 0)) {
		velocity.x = 0;
	}
	if (isDead() || velocity.x >= 4.f || velocity.x <= -4.f) {
		return;
	}
	velocity.x += x_speed;
	velocity.y += y_speed;
}

/**
 * Animacja mrugania postaci po kontakcie
 * z wrogiem
 */
void Character::updateHitAnim() {
	blood_anim_visible_time.tick();
	if (!blood_anim_visible_time.active) {
		blood_anim_visible_time.reset();
		blood_anim_cycles.tick();
		if (!blood_anim_cycles.active) {
			UNFLAG(action, BLOODING);
			//
			blood_anim_cycles.reset();
		}
	}
}

void Character::drawObject(Window*) {
	if (ai) {
		ai->drive();
	}
	if (IS_SET(action, BLOODING)) {
		updateHitAnim();
		//
		if (isDrawingBlood()) {
			/**
			 * Pobieranie ostatniego shaderu
			 */
			GLint last_program;
			glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);

			shaders[HIT_CHARACTER_SHADER]->begin();
			//
			IrregularPlatform::drawObject(NULL);
			//
			shaders[HIT_CHARACTER_SHADER]->end();

			/**
			 * Powrót do ostatniego shaderu
			 */
			glUseProgram(last_program);
		}
	} else {
		IrregularPlatform::drawObject(NULL);
	}
}

