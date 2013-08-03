/*
 * Factory.cpp
 *
 *  Created on: 30-05-2013
 *      Author: mateusz
 */
#include "Objects.hpp"
#include "Weapons.hpp"

#include "../../Tools/Logger.hpp"

ResourceFactory::ResourceFactory() :
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
void ResourceFactory::loadTexturesPack() {
	// Gracz
	readShape("gracz.txt", "player", 0);
	readShape("czaszka.txt", "cranium", 0);
	
	// Kolce
	putTexture(SPIKES_UP, readShape("kolce.txt", "spikes_up", 0));
	putTexture(SPIKES_DOWN, readShape("kolce.txt", "spikes_down", 270));
	putTexture(SPIKES_LEFT, readShape("kolce.txt", "spikes_left", -90));
	putTexture(SPIKES_RIGHT, readShape("kolce.txt", "spikes_right", 90));
	
	// Tekstury wrogów
	putTexture(SCORE, readShape("punkt.txt", "score", 0));
	putTexture(HEALTH, readShape("zycie.txt", "health", 0));
	putTexture(GHOST, readShape("wrog.txt", "enemy", 0));
	putTexture(GUN, readShape("bron.txt", "gun", 0));
	putTexture(GREEN_GUN, readShape("bron.txt", "gun2", 0));
	
	// Pozostałe ksztalty
	readShape("pocisk.txt", "bullet", 0);
	readShape("pocisk_zielony.txt", "bullet_green", 0);
	
	//
	logEvent(Logger::LOG_INFO, "Pomyślnie wczytano paczkę tekstur!");
}

void ResourceFactory::putTexture(usint id, PlatformShape* shape) {
	textures[id] = shape;
}

ResourceFactory& ResourceFactory::getIstance(pEngine* _physics) {
	static ResourceFactory factory;
	if (_physics) {
		if (!factory.physics) {
			factory.loadTexturesPack();
		}
		factory.physics = _physics;
	}
	return factory;
}

/**
 * Generowanie obiektów wraz ze skryptami
 */
Body* ResourceFactory::createObject(usint _type, float _x, float _y, float _w,
		float _h, PlatformShape* _shape, char* _script) {
	if (physics == NULL) {
		logEvent(Logger::LOG_ERROR, "Fabryka zgłasza praw fizyki brak!");
		return NULL;
	}
	
	/**
	 * Obiekt skryptu dziedziczy tylko
	 * od Body!
	 */
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
	
	/**
	 * Moby:
	 * GREEN_GUN strzela szybciej niż GUN!
	 */
	Platform* _object = NULL;
	if (_type == GUN) {
		_object = new Gun(physics, _x, _y, 16, textures[_type],
				dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(
						"bullet")), 240);
		/**
		 *
		 */
	} else if (_type == GREEN_GUN) {
		_object = new Gun(physics, _x, _y, 16, textures[_type],
				dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(
						"bullet_green")), 160);
		/**
		 *
		 */
	} else {
		_object = new Character("", _x, _y,
				_shape == NULL ? textures[_type] : _shape, Character::NONE);
		Character* character = dynamic_cast<Character*>(_object);
		switch (_type) {
			
			/**
			 * Kolce!
			 */
			case SPIKES_UP:
			case SPIKES_DOWN:
			case SPIKES_LEFT:
			case SPIKES_RIGHT: {
				character->setType(Character::SPIKES);
				
				/**
				 * Orientacja odwrotna ze względu
				 * na kąt padania gracza
				 */
				usint _orientation;
				if (_type == SPIKES_UP)
					_orientation = pEngine::DOWN;
				else if (_type == SPIKES_DOWN)
					_orientation = pEngine::UP;
				else if (_type == SPIKES_LEFT)
					_orientation = pEngine::RIGHT;
				else
					_orientation = pEngine::LEFT;
				//
				character->orientation = _orientation;
				/**
				 * Wymiary
				 */
				character->fitToWidth(24);
			}
				break;
				
				/**
				 * Życie
				 */
			case HEALTH:
				character->setType(Character::SCORE);
				character->fitToWidth(16);
				character->setStatus(health_status);
				break;
				
				/**
				 * Potwór
				 */
			case GHOST:
				character->setType(Character::ENEMY);
				character->fitToWidth(20);
				character->setStatus(ghost_enemy_status);
				character->setAI(new SnailAI(character, 1.2));
				break;
				
				/**
				 * Platforma
				 */
			case OBJECT:
				character->setType(Character::PLATFORM);
				character->fitToWidth(_w);
				break;
				
				/**
				 * Punkt
				 */
			case SCORE:
				character->setType(Character::SCORE);
				character->fitToWidth(12);
				character->setStatus(score_status);
				break;
				
				/**
				 *
				 */
			default:
				delete _object;
				//
				logEvent(Logger::LOG_WARNING, "Nieznany typ moba!");
				//
				return NULL;
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
void ResourceFactory::unloadObjects() {
	for (auto* trigger : triggers) {
		if (trigger) {
			delete trigger;
		}
	}
	triggers.clear();
	//
	for (auto* object : created) {
		if (object) {
			delete object;
		}
	}
	created.clear();
}
