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
 * Generowanie obiektu!
 */
bool ObjectFactory::createObject(usint _type, float _x, float _y, float _w,
		float _h, PlatformShape* _shape) {
	if (physics == NULL) {
		return false;
	}
	//
	Platform* _object = NULL;
	if (_type == GUN) {
		_object = new Gun(physics, _x, _y, 32, textures[_type],
				dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(
						"bullet")), 140);
		_object->setState(Body::STATIC);
		/**
		 *
		 */
	} else if (_type == GREEN_GUN) {
		_object = new Gun(physics, _x, _y, 32, textures[_type],
				dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(
						"bullet_green")), 60);
		_object->setState(Body::STATIC);
		/**
		 *
		 */
	} else {
		_object = new Character("", _x, _y, _shape == NULL ?
				textures[_type] : _shape, Character::NONE);
		Character* character = dynamic_cast<Character*>(_object);
		switch (_type) {
			case HEALTH: {
				character->setFlag(Character::SCORE);
				character->setNick("Zycie");
				character->fitToWidth(32);
				character->setStatus(health_status);
			}
				break;
				/**
				 *
				 */
			case GHOST: {
				character->setFlag(Character::ENEMY);
				character->setNick("Trup");
				character->fitToWidth(32);
				character->setStatus(ghost_enemy_status);
				character->setAI(new SnailAI(character, 3));
			}
				break;
				/**
				 *
				 */
			case OBJECT:
				character->setFlag(Character::PLATFORM);
				character->fitToWidth(_w);
				break;
				/**
				 *
				 */
			case SCORE: {
				character->setFlag(Character::SCORE);
				character->fitToWidth(26);
				character->setStatus(score_status);
			}
				break;
		}
	}
	if (_object) {
		created.push_back(_object);
		physics->insert(_object);
	}
	return true;
}

/**
 * Kasowanie całego poziomu!
 */
void ObjectFactory::unloadObjects() {
	for (usint i = 0; i < created.size(); ++i) {
		if (created[i]) {
			delete created[i];
		}
	}
	created.clear();
}
