/*
 * Characters.cpp
 *
 *  Created on: 23-03-2013
 *      Author: mati
 */
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
void generateBlood(usint dir, pEngine* physics, Body* body, usint count) {
	float cx, cy, angle;
	/**
	 * Generowanie cząsteczek krwii w postaci eksplozji
	 */
	for (usint i = 0; i < count; ++i) {
		angle = 2.0f * 3.1415926f * (float) i / (float) 36;
		cx = body->w / 2 * cosf(angle);
		cy = body->w / 2 * sinf(angle);
		//
		float size = getIntRandom<int>(3, 6);
		//
		Vector<float> vec(cosf(angle) * 16, sinf(angle) * 25);
		if (dir != pEngine::NONE) {
			/**
			 * Kierunek rozprucia odwrotny
			 * do kierunku uderzenia!
			 */
			switch (dir) {
				case pEngine::LEFT:
					vec.x = -abs(vec.x);
					break;

				case pEngine::RIGHT:
					vec.x = abs(vec.x);
					break;

				case pEngine::UP:
					vec.y = abs(vec.y);
					break;

				case pEngine::DOWN:
					vec.y = -abs(vec.y);
					break;
					/**
					 *
					 */
				default:
					break;
			}
		}
		Platform* platform = new Platform(body->x + cx, body->y + cy, size,
		                                  size, oglWrapper::RED, Body::NONE);
		//
		platform->setMaxLifetime(size * 140);
		platform->setBorder(false, false, false, false);
		platform->setFillType(Platform::FILLED);
		//
		platform->layer = STATIC_LAYER + 1;
		platform->velocity = vec;
		//
		physics->insert(platform);
	}
}

//------------------------

Character::Character(const string& _nick, float _x, float _y,
                     PlatformShape* _shape, usint _type) :
		IrregularPlatform(_x, _y, true, _shape),
		nick(Color(255, 255, 255), _nick, GLUT_BITMAP_HELVETICA_12, 12),
		jumping(true),
		status(NULL, MAX_LIVES, false, 0, 0, _x, _y),
		ai(NULL),
		//
		source_color(col),
		actual_anim_time(0),
		anim_time(9),
		actual_cycles(0),
		anim_cycles(8),
		//
		hit(false) {
	type = _type;
}

/**
 * Uderzenie - zmniejszenie życia
 */
void Character::hitMe(pEngine* physics) {
	hit = true;
	//
	generateBlood(pEngine::NONE, physics, this, 5);
}

/**
 * Eksplozja
 */
void Character::die(pEngine* physics, usint _dir) {
	if (status.health == DEATH) {
		return;
	}
	/**
	 * TRRUP
	 */
	status.health = DEATH;
	//
	generateBlood(_dir, physics, this, 30);
	//
	setState(Body::HIDDEN);
}

/**
 * Kolizja gracza
 */
void Character::catchCollision(pEngine* physics, usint dir, Body* body) {
	if (ai) {
		ai->getCollision(physics, dir, body);
	}
	if ((type == Body::HERO || type == Body::ENEMY)
	        && dir == pEngine::DOWN&& !IS_SET(body->state, Body::HIDDEN)) {
		/**
		 * Wektor ruchu został odwrócony!
		 */
		if (velocity.y < -9 && status.health > 0) {
			die(physics, dir);
		}
		jumping = false;
	}
	if (type != HERO) {
		return;
	}
	/**
	 * Akcje gracza!
	 */
	if (body->type == Body::TRIGGER) {
		dynamic_cast<Trigger*>(body)->generate();
		return;
	}
	Character* enemy = dynamic_cast<Character*>(body);
	if (!enemy) {
		return;
	}
	switch (enemy->type) {
		case SCORE:
			status += enemy->status;
			if (status.health > MAX_LIVES) {
				status.health = MAX_LIVES;
			}
			//
			body->destroyed = true;
			break;

			/**
			 *
			 */
		case ENEMY:
			if (dir == pEngine::DOWN || dir == pEngine::UP) {
				status += enemy->status;
				velocity.invert();
				//
				body->destroyed = true;
			} else {
				status -= enemy->status;
				hitMe(physics);
			}
			break;

			/**
			 *
			 */
		case BULLET:
			if (status.health > 0) {
				status.health -= 1;
			}
			hitMe(physics);
			velocity.invert();
			//
			body->destroyed = true;
			break;

			/**
			 *
			 */
		case SPIKES:
			if (status.health > 0) {
				status.health -= 1;
			}
			hitMe(physics);
			jump(13, true);
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
 * Skok z określoną prędkością!
 */
void Character::jump(float _y_speed, bool _force) {
	if (!jumping || _force) {
		jumping = true;
		velocity.y = -_y_speed;
	}
}

void Character::move(float x_speed, float y_speed) {
	if ((x_speed > 0 && velocity.x < 0) || (x_speed < 0 && velocity.x > 0)) {
		velocity.x = 0;
	}
	if (velocity.x >= 4.f || velocity.x <= -4.f) {
		return;
	}
	velocity.x += x_speed;
	velocity.y += y_speed;
}

/**
 * Animacja mrugania postaci po kontakcie
 * z wrogiem
 */
void Character::drawHitAnimation() {
	if (!hit) {
		return;
	}
	actual_anim_time++;
	if (actual_anim_time >= anim_time) {
		actual_anim_time = 0;
		actual_cycles++;
		if (actual_cycles > anim_cycles) {
			actual_cycles = hit = false;
			col = source_color;
		} else {
			if (actual_cycles % 2) {
				col = source_color;
			} else {
				col = oglWrapper::RED;
			}
		}
	}
}

void Character::drawObject(Window*) {
	if (ai) {
		ai->drive();
	}
	drawHitAnimation();
	IrregularPlatform::drawObject(NULL);
	/**
	 * Nieoptymalne rozwiązanie..
	 * Za dużo push!
	 */
	if (!nick.getString()->empty()) {
		glPushMatrix();
		glTranslatef(x, y, 1);
		nick.printText(w / 2 - nick.getScreenLength() / 2, -10);
		glPopMatrix();
	}
}

