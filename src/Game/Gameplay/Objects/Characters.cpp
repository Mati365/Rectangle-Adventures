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

//------------------------

Character::Character(const string& _nick, float _x, float _y,
		PlatformShape* _shape, usint _flag) :
		IrregularPlatform(_x, _y, true, _shape),
		nick(Color(255, 255, 255), _nick, GLUT_BITMAP_HELVETICA_12, 12),
		jumping(true),
		status(NULL, MAX_LIVES, false, 0, 0, _x, _y),
		ai(NULL),
		//
		source_color(col),
		actual_anim_time(0),
		anim_time(5),
		actual_cycles(0),
		anim_cycles(8),
		//
		hit(false) {
	flag = _flag;
}

void Character::catchCollision(pEngine* physics, usint dir, Body* body) {
	if (ai) {
		ai->getCollision(physics, dir, body);
	}
	if (dir == pEngine::DOWN && IS_SET(body->flag, Body::STATIC)) {
		jumping = false;
	}
	if (flag != HERO) {
		return;
	}
	/**
	 * Akcje gracza!
	 */
	Character* enemy = dynamic_cast<Character*>(body);
	switch (body->flag) {
		case SCORE:
			status += enemy->status;
			if (status.health > MAX_LIVES) {
				status.health = MAX_LIVES;
			}
			body->destroyed = true;
			break;
			/**
			 *
			 */
		case ENEMY:
			if (dir == pEngine::DOWN || dir == pEngine::UP) {
				status += enemy->status;
				jump(15);
				body->destroyed = true;
			} else {
				status -= enemy->status;
				enableHitAnim();
			}
			break;
			/**
			 *
			 */
		case BULLET:
			if (status.health > 0) {
				status.health -= 1;
			}
			enableHitAnim();
			jump(15);
			body->destroyed = true;
			break;
	}
}

/**
 * Skok z określoną prędkością!
 */
void Character::jump(float _y_speed) {
	if (!jumping) {
		jumping = true;
		velocity.y = -_y_speed;
	}
}

void Character::move(float x_speed, float y_speed) {
	if ((x_speed > 0 && velocity.x < 0) || (x_speed < 0 && velocity.x > 0)) {
		velocity.x = 0;
	}
	if (velocity.x >= 6.f || velocity.x <= -6.f) {
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
		nick.printText(w / 2 - nick.getScreenLength() / 2, -20);
		glPopMatrix();
	}
}

