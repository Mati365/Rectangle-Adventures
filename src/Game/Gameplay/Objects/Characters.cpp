/*
 * Characters.cpp
 *
 *  Created on: 23-03-2013
 *      Author: mati
 */
#include <GL/glew.h>

#include "Objects.hpp"
#include "Weapons.hpp"

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
void generateExplosion(pEngine* physics, const Rect<float>& body, usint count,
		const Color& col, float min_size, float max_size,
		const Vector<float>& velocity, float life_time, usint state) {
	float angle;

	/**
	 * Generowanie cząsteczek krwii w postaci eksplozji
	 * Różny promień
	 */
	for (usint i = 0; i < count; ++i) {
		angle = TO_RAD(getIntRandom<int>(0.f, 360.f));
		//
		Vector<float> r(
				getIntRandom<int>(0, body.w / 2),
				getIntRandom<int>(0, body.h / 2));

		r.x *= cosf(angle);
		r.y *= sinf(angle);
		//
		float size = getIntRandom<int>(min_size, max_size);
		//
		Platform* platform = new Platform(
				body.x + r.x,
				body.y + r.y,
				size,
				size,
				col,
				state);
		//
		platform->setMaxLifetime(life_time == -1 ? size * 140 : life_time);
		platform->setBorder(false, false, false, false);
		platform->setFillType(Platform::FILLED);
		//
		platform->layer = STATIC_LAYER + 1;
		platform->velocity = Vector<float>(
				cosf(angle) * velocity.x,
				sinf(angle) * velocity.y);
		//
		physics->insert(platform);
	}
}

//------------------------

Character::Character(const string& _nick, float _x, float _y,
		PlatformShape* _shape, usint _type) :
				IrregularPlatform(_x, _y, true, _shape),
				action(JUMPING),
				status(MAX_LIVES, false, 0, 0, _x, _y),
				ai(NULL),
				//
				blood_anim_visible_time(11),
				blood_anim_cycles(8) {
	type = _type;
	//
	addCheckpoint(true);
}

/**
 * Uderzenie - zmniejszenie życia
 */
void Character::hitMe(pEngine* physics) {
	ADD_FLAG(action, BLOODING);
	//
	generateExplosion(
			physics,
			static_cast<Rect<float> >(*this),
			5,
			oglWrapper::RED,
			3,
			6,
			Vector<float>(8, 12));
	//
	playResourceSound(SPIKES_SOUND);
}

/**
 * Eksplozja
 */
void Character::die(pEngine* physics) {
	if (isDead()) {
		return;
	}
	/**
	 * TRRUP
	 */
	status.health = DEATH;

	/**
	 * Generowanie eksplozji
	 */
	generateExplosion(
			physics,
			static_cast<Rect<float> >(*this),
			30,
			oglWrapper::RED,
			3,
			6,
			Vector<float>(8, 12));

	/**
	 * Zmiana kształtu na trup
	 */
	float _last_w = w;

	setShape(getShapePointer("cranium"));
	fitToWidth(_last_w * 0.6);

	/**
	 * Dźwięk śmierci ;_;
	 */
	playResourceSound(DIE_SOUND);
}

/**
 * Dodawanie checkpointu
 */
void Character::addCheckpoint(bool _reload_map) {
	// Mobom nie potrzeba checkpointów
	if (type != HERO) {
		return;
	}

	status.start_pos = (Vector<float> ) *this;
	//
	last_checkpoint.last_status = status;
	last_checkpoint.reload_map = _reload_map;
	//
	last_checkpoint.last_status.health = MAX_LIVES;

	/**
	 * Tooltip musi być xD
	 */
	if (_reload_map) {
		addTooltip("-respawn", oglWrapper::RED);
	} else {
		addTooltip("+respwan", oglWrapper::PURPLE);
	}
}

/**
 * Odzyskanie z ostatniego checkpoint'a
 */
void Character::recoverFromCheckpoint(pEngine* physics) {
	status = last_checkpoint.last_status;

	// Resetowanie wyglądu
	float _last_w = w;

	setShape(getShapePointer("player"));
	fitToWidth(_last_w / 0.6);

	// Resetowanie pozycji
	velocity.x = velocity.y = 0;
	x = status.start_pos.x;
	y = status.start_pos.y;

	// Odjąć trza życia i punktów
	status.score =
			status.score - MAX_SCORE * 0.1 > 0 ?
					status.score - MAX_SCORE * 0.1 : 0;
	status.health -= 1;

	hitMe(physics);
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
			die(physics);
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
		 * Strefa śmierci ;_;
		 */
		case KILLZONE:
			status.health = 0;
			break;

			/**
			 * Na lianie tylo w dół!
			 */
		case LIANE:
		case LADDER: {
			float _max_speed = physics->getGravitySpeed() * 2;
			//
			if (velocity.y > _max_speed) {
				velocity.y = _max_speed;
			} else if (velocity.y < 0) {
				if (enemy->type == LADDER && velocity.y < -_max_speed * 2.f) {
					velocity.y = -_max_speed * 2.f;
				} else if (enemy->type == LIANE) {
					velocity.y = physics->getGravitySpeed() / 2;
				}
			}
			if (velocity.x >= -0.5 && velocity.x <= 0.5
					&& (velocity.y == _max_speed
							|| velocity.y == -_max_speed * 2.f)) {
				x = body->x + body->w / 2 - w / 2;
			}
			//
			ADD_FLAG(action, CLIMBING);
		}
			break;

			/**
			 *
			 */
		case SCORE: {
			status += enemy->status;
			if (status.health > MAX_LIVES) {
				status.health = MAX_LIVES;
			}
			if (status.score > MAX_SCORE) {
				status.score = MAX_SCORE;
			}
			Color col =
					*dynamic_cast<IrregularPlatform*>(body)->getShape()->getMainColor();
			//
			generateExplosion(
					physics,
					*dynamic_cast<Rect<float>*>(body),
					6,
					col,
					2,
					3,
					Vector<float>(8, 12));
			//
			body->destroyed = true;
			//
			if (enemy->status.health > 0 || enemy->status.score) {
				playResourceSound(SCORE_SOUND);
			}
			addTooltip(enemy->status.health > 0 ? "+1hp" : "+1exp", col);
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
			status += enemy->status;
			body->destroyed = true;
			//
			hitMe(physics);
			dodge(dir);
			break;

			/**
			 *
			 */
		case SPIKES:
			if (body->orientation != invertDir(dir)) {
				break;
			}
			status += enemy->status;
			//
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
		die(physics);
	}
}

/**
 * Unik
 */
void Character::dodge(usint _dir) {
	float _speed = velocity.y * 0.5f;
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
	//
	addTooltip("Ouch!", oglWrapper::WHITE);
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

/**
 * Rysowanie tooltipów
 */
#define TOOLTIP_SPEED -1

void Character::drawTooltips() {
	if (tooltips.empty()) {
		return;
	}

	/**
	 * Wyłączanie starego
	 * shaderu
	 */
	GLint last_program = Shader::getLastShader();
	glUseProgram(0);

	/**
	 * Gridy lub interatory zbyt
	 * zmniejszą prędkość
	 */
	for (usint i = 0; i < tooltips.size();) {
		_Tooltip& object = tooltips[i];

		// I przy okazji to odświeżanie
		object.pos.y += TOOLTIP_SPEED;

		if (object.life_timer.active) {
			object.life_timer.tick();
			//
			object.text.getColor()->a =
					255.f
							* (1.f
									- (float) object.life_timer.cycles_count
											/ (float) object.life_timer.max_cycles_count);
		} else {
			tooltips.erase(tooltips.begin() + i);
			continue;
		}

		// Rysowanie
		object.text.printText(
				object.pos.x - object.text.getScreenLength() / 2,
				object.pos.y);

		++i;
	}

	/**
	 * Przywracanie starego shaderu
	 */
	glUseProgram(last_program);
}

/**
 * Malowanie całego gracza
 */
void Character::drawObject(Window*) {
	if (ai) {
		ai->drive();
	}
	if (IS_SET(action, BLOODING)) {
		updateHitAnim();
		//
		if (isBlooding()) {
			/**
			 * Pobieranie ostatniego shaderu
			 */
			GLint last_program = Shader::getLastShader();

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
	drawTooltips();
}

