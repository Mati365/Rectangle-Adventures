/*
 * Factory.cpp
 *
 *  Created on: 30-05-2013
 *      Author: mateusz
 */
#include "Objects.hpp"
#include "Weapons.hpp"

#include "../../Tools/Logger.hpp"

ObjectFactory::ObjectFactory() :
		// Statusy!
		health_status("health_status", 1, false, 0, 1),
		score_status("score_status", 0, false, 0, 15),
		ghost_enemy_status("health_status", 1, false, 0, 50),
		//
		physics(NULL) {
}

/**
 * Mapowanie podstawowych tekstur w grze!
 */
void ObjectFactory::loadGameTexturePack() {
	readShape("pocisk_zielony.txt", "bullet_green");
	readShape("pocisk.txt", "bullet");
	readShape("gracz.txt", "player");
	//
	putTexture(ObjectFactory::SCORE, readShape("punkt.txt", "score"));
	putTexture(ObjectFactory::HEALTH, readShape("zycie.txt", "health"));
	putTexture(ObjectFactory::GHOST, readShape("wrog.txt", "enemy"));
	putTexture(ObjectFactory::GUN, readShape("bron.txt", "gun"));
	putTexture(ObjectFactory::GREEN_GUN, readShape("bron.txt", "gun2"));
	//
	logEvent(Logger::LOG_INFO, "Pomyślnie wczytano paczkę postaci!");
}

ObjectFactory& ObjectFactory::getIstance(pEngine* _physics) {
	static ObjectFactory factory;
	if (_physics) {
		if (!factory.physics) {
			factory.loadGameTexturePack();
		}
		factory.physics = _physics;
	}
	return factory;
}

void ObjectFactory::putTexture(usint id, PlatformShape* shape) {
	textures[id] = shape;
}

/**
 * Generowanie skryptów!
 */

/**
 * Generowanie obiektu!
 */
Body* ObjectFactory::createObject(usint _type, float _x, float _y, float _w,
									float _h, PlatformShape* _shape,
									char* _script) {
	if (physics == NULL) {
		logEvent(Logger::LOG_ERROR, "Fabryka zgłasza praw fizyki brak!");
		return NULL;
	}
	//
	if (_type == SCRIPT_BOX) {
		if (!_script) {
			logEvent(Logger::LOG_WARNING, "Nie mogę utworzyć triggera!");
			return NULL;
		}
		Trigger* trigger = new Trigger(
				Interpreter::getIstance().compile(_script), _x, _y, _w, _h);
		triggers.push_back(trigger);
		physics->insert(trigger);
		//
		return trigger;
	}
	Platform* _object = NULL;
	if (_type == GUN) {
		_object = new Gun(
				physics,
				_x,
				_y,
				16,
				textures[_type],
				dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(
						"bullet")),
				240);
		/**
		 *
		 */
	} else if (_type == GREEN_GUN) {
		_object = new Gun(
				physics,
				_x,
				_y,
				16,
				textures[_type],
				dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(
						"bullet_green")),
				160);
		/**
		 *
		 */
	} else {
		_object = new Character("", _x, _y, _shape == NULL ?
				textures[_type] : _shape,
								Character::NONE);
		Character* character = dynamic_cast<Character*>(_object);
		switch (_type) {
			case HEALTH: {
				character->setType(Character::SCORE);
				character->setNick("Zycie");
				character->fitToWidth(16);
				character->setStatus(health_status);
			}
				break;
				/**
				 *
				 */
			case GHOST: {
				character->setType(Character::ENEMY);
				character->setNick("Kupa");
				character->fitToWidth(20);
				character->setStatus(ghost_enemy_status);
				character->setAI(new SnailAI(character, 1.2));
			}
				break;
				/**
				 *
				 */
			case OBJECT:
				character->setType(Character::PLATFORM);
				character->fitToWidth(_w);
				break;
				/**
				 *
				 */
			case SCORE: {
				character->setType(Character::SCORE);
				character->fitToWidth(12);
				character->setStatus(score_status);
			}
				break;
		}
	}
	if (_object) {
		created.push_back(_object);
		physics->insert(_object);
	}
	return _object;
}

/**
 * Kasowanie całego poziomu!
 */
void ObjectFactory::unloadObjects() {
	for (usint i = 0; i < triggers.size(); ++i) {
		if (triggers[i]) {
			delete triggers[i];
		}
	}
	triggers.clear();
	//
	for (usint i = 0; i < created.size(); ++i) {
		if (created[i]) {
			delete created[i];
		}
	}
	created.clear();
}
