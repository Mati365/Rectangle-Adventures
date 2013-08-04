/*
 * Factory.cpp
 *
 *  Created on: 30-05-2013
 *      Author: mateusz
 */
#include "Objects.hpp"
#include "Weapons.hpp"

#include "../../Tools/Logger.hpp"

/**
 * Podstawowe obiekty w fabryce!!!
 */
ResourceFactory::FactoryType ResourceFactory::factory_types[] =
		{
			{ SPIKES, pEngine::RIGHT, 90, "kolce.txt", "spikes_right" },
			{ SPIKES, pEngine::LEFT, -90, "kolce.txt", "spikes_left" },
			{ SPIKES, pEngine::UP, 0, "kolce.txt", "spikes_up" },
			{ SPIKES, pEngine::DOWN, 180, "kolce.txt", "spikes_down" },
			//
			{ SCORE, pEngine::NONE, 0, "punkt.txt", "score" },
			{ HEALTH, pEngine::NONE, 0, "zycie.txt", "health" },
			{ GHOST, pEngine::NONE, 0, "wrog.txt", "enemy" },
			{ GUN, pEngine::NONE, 0, "bron.txt", "gun" } };

/**
 * Konstruktor prywatny!
 */
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
 *
 * UWAGA! KOLEJNE KIERUNKI TEKSTUR SĄ FLAGAMI +100 od
 * PIERWOTNEJ!!!
 */
#define DIRECTION_TEXTURE_ADDITION 100

// generowanie kolejnych id dla poszczególnych orientacji
usint ResourceFactory::genTextureID(usint _type, usint _orientation) const {
	return 5 * _type + DIRECTION_TEXTURE_ADDITION + _orientation;
}

void ResourceFactory::loadTexturesPack() {
	// Gracz
	readShape("gracz.txt", "player", 0);
	readShape("czaszka.txt", "cranium", 0);
	
	// Tekstury mobów
	for (FactoryType& factory_object : factory_types) {
		putTexture(
				genTextureID(factory_object.type, factory_object.orientation),
				readShape(
						factory_object.file_name,
						factory_object.resource_label,
						factory_object.rotation));
	}
	
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
		float _h, PlatformShape* _shape, char* _script, usint _orientation) {
	if (physics == NULL) {
		logEvent(Logger::LOG_ERROR, "Fabryka zgłasza praw fizyki brak!");
		return NULL;
	}
	
	/**
	 * Identyfikator teksturyt obikektu!
	 */
	usint _texture_id = genTextureID(_type, _orientation);
	
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
				Interpreter::getIstance().compile(_script),
				_x,
				_y,
				_w,
				_h);
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
		_object = new Gun(
				physics,
				_x,
				_y,
				16,
				textures[_texture_id],
				dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(
						"bullet")),
				240);
		/**
		 *
		 */
	} else {
		_object = new Character(
				"",
				_x,
				_y,
				_shape == NULL ? textures[_texture_id] : _shape,
				Character::NONE);
		Character* character = dynamic_cast<Character*>(_object);
		character->orientation = _orientation;
		//
		switch (_type) {
			
			/**
			 * Kolce!
			 */
			case SPIKES:
				character->setType(Character::SPIKES);
				character->fitToWidth(23);
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
