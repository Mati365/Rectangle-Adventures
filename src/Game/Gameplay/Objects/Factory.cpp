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
		// typ orientacja obrot szerokosc nazwa_pliku nazwa_zasobu
			{ SPIKES, pEngine::RIGHT, 90.f, 20, "kolce.txt", "spikes_right" },
			{ SPIKES, pEngine::LEFT, -90.f, 20, "kolce.txt", "spikes_left" },
			{ SPIKES, pEngine::UP, 0.f, 23, "kolce.txt", "spikes_up" },
			{ SPIKES, pEngine::DOWN, 180.f, 23, "kolce.txt", "spikes_down" },
			//
			{ SCORE, pEngine::NONE, 0.f, 12, "punkt.txt", "score" },
			{ HEALTH, pEngine::NONE, 0.f, 16, "zycie.txt", "health" },
			{ GHOST, pEngine::NONE, 0.f, 6, "wrog.txt", "enemy" },
			//
			{ GUN, pEngine::RIGHT, 90.f, 12, "bron.txt", "gun_right" },
			{ GUN, pEngine::LEFT, -90.f, 12, "bron.txt", "gun_left" },
			{ GUN, pEngine::UP, 0.f, 16, "bron.txt", "gun_up" },
			{ GUN, pEngine::DOWN, 180.f, 16, "bron.txt", "gun_down" },
			//
			{ LADDER, pEngine::NONE, 0.f, 23, "drabina.txt", "stairs" },
			{ LIANE, pEngine::NONE, 0.f, 16, "liana.txt", "liane" },
			// Szerokość bez znaczenia!
			{ BULLET, pEngine::RIGHT, 90.f, 0, "pocisk.txt", "bullet_right" },
			{ BULLET, pEngine::LEFT, -90.f, 0, "pocisk.txt", "bullet_left" },
			{ BULLET, pEngine::UP, 0.f, 0, "pocisk.txt", "bullet_up" },
			{ BULLET, pEngine::DOWN, 180.f, 0, "pocisk.txt", "bullet_down" } };

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
 * UWAGA! KOLEJNE KIERUNKI TEKSTUR SĄ FLAGAMI +MAX_ILOŚĆ od
 * PIERWOTNEJ!!!
 */
#define MAX_TEXTURES_COUNT 300

// generowanie kolejnych id dla poszczególnych orientacji
usint ResourceFactory::genTextureID(usint _type, usint _orientation) const {
	return 5 * _type + MAX_TEXTURES_COUNT + _orientation;
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
	 * Lawa
	 */
	if (_type == LAVA) {
		Lava* lava = new Lava(_x, _y);
		//
		addBody(lava);
	}

	/**
	 * Moby:
	 * Działko
	 */
	/**
	 * Przeszukiwanie bazy obiektów
	 */
	FactoryType* _factory_type = getFactoryType(_type, _orientation);
	usint _width = _factory_type ? _factory_type->width : 0;

	Platform* _object = NULL;

	if (_type == KILLZONE) {
		_object = new Platform(_x, _y, _w, _h, oglWrapper::WHITE, Body::HIDDEN);
	} else if (_type == GUN) {
		_object = new Gun(
				physics,
				_x,
				_y,
				textures[_texture_id],
				{ textures[genTextureID(BULLET, 1)], textures[genTextureID(
							BULLET,
							2)],
					textures[genTextureID(BULLET, 3)], textures[genTextureID(
							BULLET,
							4)] },
				340);
		_object->orientation = _orientation;
		dynamic_cast<Gun*>(_object)->fitToWidth(_width);
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
		character->fitToWidth(_width);

		character->orientation = _orientation;
		/**
		 * Typ obiektu w fabryce to nie typ obiektu
		 * w fizyce!
		 */
		switch (_type) {
			/**
			 * Schody!
			 */
			case LADDER:
				character->setType(Character::LADDER);
				character->setState(Body::BACKGROUND);
				break;

				/**
				 * Liana!
				 */
			case LIANE:
				character->setType(Character::LIANE);
				character->setState(Body::BACKGROUND);
				break;

				/**
				 * Kolce!
				 */
			case SPIKES:
				character->setType(Character::SPIKES);
				break;

				/**
				 * Życie
				 */
			case HEALTH:
				character->setType(Character::SCORE);
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
	//
	addBody(_object);
	//
	return _object;
}

void ResourceFactory::addBody(Body* _object) {
	if (_object) {
		created.push_back(_object);
		physics->insert(_object);
	}
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
