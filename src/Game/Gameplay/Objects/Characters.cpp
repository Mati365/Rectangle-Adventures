/*
 * Characters.cpp
 *
 *  Created on: 23-03-2013
 *      Author: mati
 */
#include <GL/glew.h>

#include "Objects.hpp"
#include "Weapons.hpp"

#include "../../Resources/Data/SoundManager.hpp"

#include "../Particle/Particle.hpp"
#include "../Gameplay.hpp"

using namespace Gameplay;

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
		platform->layer = BLOOD_LAYER;
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
				
				// Timery
				blood_anim(8),
				levitation_timer(
						DEFAULT_LEVITATION_DURATION,
						getIntRandom<int>(0, DEFAULT_LEVITATION_DURATION)),
				start_pos(_x, _y),
				// Serce
				diastole(false),
				heart_timer(
						HEART_SHRINK_DURATION,
						getIntRandom(0, HEART_SHRINK_DURATION)),
				// Uśpienie
				sleep_timer(300),
				zzz_delay(30) {
	type = _type;
	levitation_timer.loop = true;
	
	// Uśpienie
	blood_anim.sleep_beetwen_cycle = 11;
	heart_timer.sleep_beetwen_cycle = 8;
	
	// Checkpoint
	addCheckpoint(true);
}

/**
 * Uderzenie - zmniejszenie życia
 */
void Character::hitMe() {
	ADD_FLAG(action, BLOODING);
	//
	blood_anim.reset();
	generateExplosion(
			physics,
			static_cast<Rect<float> >(*this),
			5,
			oglWrapper::RED,
			3,
			6,
			Vector<float>(8, 12));
	//
	SoundManager::getInstance().playResourceSound(SoundManager::SPIKES_SOUND);
}

/**
 * Eksplozja
 */
void Character::die() {
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
	SoundManager::getInstance().playResourceSound(SoundManager::DIE_SOUND);
}

/**
 * Odblokowywanie spania
 */
void Character::updateSleeping() {
	// Upływ czasu nieaktywności
	sleep_timer.tick();
	if (!sleep_timer.active) {
		// Emitowanie Zzz
		zzz_delay.tick();
		if (!zzz_delay.active) {
			addTooltip(
					"Zzz",
					oglWrapper::WHITE,
					0,
					0,
					zzz_delay.max_cycles_count / 3 * 10,
					TOOLTIP_SPEED / 2.f);
			zzz_delay.reset();
		}
	}
	ADD_FLAG(action, SLEEPING);
}

/**
 * Resetowanie spania
 */
void Character::resetSleeping() {
	sleep_timer.reset();
	//
	UNFLAG(action, SLEEPING);
}

/**
 * Napis nad graczem
 */
void Character::addTooltip(const char* _text, const Color& _col,
		float _x_correction, float _y_correction, usint _life_time,
		float _speed) {
	tooltips.push_back(
			_Tooltip(
					_text,
					Vector<float>(x + _x_correction, y + _y_correction),
					_col,
					_life_time,
					_speed));
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
void Character::recoverFromCheckpoint(MapINFO* map) {
	if (type != HERO) {
		return;
	}
	status = last_checkpoint.last_status;
	
	// Resetowanie wyglądu
	setShape(map->hero_shape);
	fitToWidth(map->hero_bounds.w);
	
	// Resetowanie pozycji
	velocity.x = velocity.y = 0;
	x = status.start_pos.x;
	y = status.start_pos.y;
	
	// Odjąć trza życia i punktów
	status.score =
			status.score - MAX_SCORE * 0.1 > 0 ?
					status.score - MAX_SCORE * 0.1 : 0;
	status.health -= 1;
	
	hitMe();
}

/**
 * Kolizja gracza
 */
void Character::catchPlayerCollision(pEngine* physics, usint dir, Body* body) {
	/**
	 * Tylko gracz akceptuje reszte kolizji!
	 */
	if (type != HERO) {
		return;
	}
	
	// Portale
	if (body->type == Body::PORTAL) {
		dynamic_cast<Portal*>(body)->enter(this, dir);
		return;
	}
	
	// Skrypty
	if (body->type == Body::TRIGGER) {
		dynamic_cast<Trigger*>(body)->generate();
		return;
	}
	
	// Reset flag
	UNFLAG(action, CLIMBING);
	
	// Akcje gracza
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
				SoundManager::getInstance().playResourceSound(
						SoundManager::SCORE_SOUND);
			}
			addTooltip(enemy->status.health > 0 ? "+1hp" : "+1exp", col);
		}
			break;
			
			/**
			 *
			 */
		case ENEMY:
			if (dir == pEngine::DOWN || dir == pEngine::UP) {
				body->destroyed = true;
			} else {
				status += enemy->status;
				hitMe();
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
			hitMe();
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
			hitMe();
			dodge(dir);
			break;
			
			/**
			 *
			 */
		default:
			break;
	}
	if (status.health == 0) {
		die();
	}
}

/**
 * Kolizje wszystkich obiektów
 */
void Character::catchCollision(pEngine* physics, usint dir, Body* body) {
	if (!body) {
		return;
	}
	/**
	 * Callbackowe kolizje dla mobów,
	 * nie wiadomo kto pierwszy odebrał
	 * kolizje gracz czy mob, dlatego
	 * lepiej upewnić się że np. gracz ją
	 * dostał
	 */
	bool is_hero = type == HERO;
	
	if (!is_hero) {
		switch (type) {
			/**
			 *
			 */
			case SPIKES:
				if (dir == pEngine::DOWN && orientation == pEngine::DOWN
						&& !body->collisions[pEngine::UP - 1]) {
					body->catchCollision(physics, invertDir(dir), this);
				}
				break;
				
				/**
				 *
				 */
			case ENEMY:
				if (body->type == HERO) {
					body->catchCollision(physics, invertDir(dir), this);
				}
				break;
				
				/**
				 *
				 */
			default:
				break;
		}
	}
	
	// Test martwości
	if (isDead()) {
		return;
	}
	
	// Ginąć może nie tylko gracz
	if (dir == pEngine::DOWN && !IS_SET(body->state, Body::HIDDEN)
			&& (type == Body::HERO || type == Body::ENEMY)) {
		// Zryte formatowanie
		if (velocity.y < -9 && status.health > 0) {
			die();
			return;
		}
		UNFLAG(action, JUMPING);
	}
	
	// Kolizje gracza
	catchPlayerCollision(physics, dir, body);
}

/**
 * No to obiekt odświeżamy..
 */
void Character::updateMe() {
	switch (type) {
		/**
		 *
		 */
		case HERO:
			updateSleeping();
			break;
			
			/**
			 *
			 */
		case SCORE: {
			levitation_timer.tick();
			
			/**
			 * Lewitacja to 1/3 wysokości obiektu
			 * Obiekt musi być BACKGROUND
			 * Sinus być chropowaty
			 */
			if (levitation_timer.cycles_count
					>= levitation_timer.max_cycles_count / 2) {
				// Opdanie
				y =
						start_pos.y + h / 1.5f
								- h
										* ((float) levitation_timer.cycles_count
												/ (float) levitation_timer.max_cycles_count);
			} else {
				// Wznoszenie się
				y =
						start_pos.y + h / 1.5f
								- h
										* (1.f
												- (float) levitation_timer.cycles_count
														/ (float) levitation_timer.max_cycles_count);
			}
			
			/**
			 * Kurczenie się serca
			 */
			if (factory_type == ResourceFactory::HEALTH) {
				static float orginal_width =
						ResourceFactory::getFactoryTemplate(
								factory_type,
								orientation)->width;
				heart_timer.tick();
				if (!heart_timer.active) {
					heart_timer.reset();
					diastole = !diastole; // rozkurcz
				}
				
				// Wyliczanie nowych rozmiarów
				float new_width =
						diastole ?
								orginal_width - heart_timer.cycles_count :
								orginal_width - HEART_SHRINK_DURATION
										+ heart_timer.cycles_count;
				
				// Zmiana rozmiaru i centrowanie
				x = start_pos.x + orginal_width / 2 - new_width / 2;
				fitToWidth(new_width);
			}
		}
			break;
			
			/**
			 *
			 */
		case ENEMY:
			// A co jeśli kierunku nie ma?
			if (orientation == pEngine::NONE) {
				orientation = getIntRandom(
						(usint) pEngine::RIGHT,
						(usint) pEngine::LEFT);
			}
			
			// Kolizja!
			if (collisions[orientation - 1]) {
				orientation = invertDir(orientation);
			}
			
			// Poruszanie się
			switch (orientation) {
				case pEngine::LEFT:
					velocity.x = -2.f;
					break;
					
				case pEngine::RIGHT:
					velocity.x = 2.f;
					break;
			}
			break;
			
			/**
			 *
			 */
		default:
			break;
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
	resetSleeping();
	//
	if (!isDead() && (!IS_SET(action, JUMPING) || _force)) {
		if (!IS_SET(action, CLIMBING) && !collisions[pEngine::DOWN - 1]) {
			return;
		}
		ADD_FLAG(action, JUMPING);
		velocity.y = -_y_speed;
		//
		if (!_force && !IS_SET(action, CLIMBING)) {
			SoundManager::getInstance().playResourceSound(
					SoundManager::JUMP_SOUND);
		}
	}
}

/**
 * Poruszanie się
 */
void Character::move(float x_speed, float y_speed) {
	resetSleeping();
	//
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
	blood_anim.tick();
	if (!blood_anim.active) {
		UNFLAG(action, BLOODING);
	}
}

/**
 * Rysowanie tooltipów
 */
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
		object.pos.y += object.speed;
		
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
	// Odświeżanie obiektu
	updateMe();
	
	//
	glLineWidth(1.f);
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
			IrregularPlatform::drawObject(nullptr);
			//
			shaders[HIT_CHARACTER_SHADER]->end();
			
			/**
			 * Powrót do ostatniego shaderu
			 */
			glUseProgram(last_program);
		}
	} else {
		IrregularPlatform::drawObject(nullptr);
	}
	drawTooltips();
}

