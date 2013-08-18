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
 * Generowanie statusów
 */

map<usint, ResourceFactory::_FactoryStatus> ResourceFactory::factory_status;

void ResourceFactory::generateChracterStatus(usint _hard_level) {
	factory_status.clear();

	// PUNKT
	factory_status[SCORE] = {
		Character::SCORE, Body::BACKGROUND, NULL, true,
		CharacterStatus(0, false, 0, 1)
	};

	// ŻYCIE
	factory_status[HEALTH] = {
		Character::SCORE, Body::BACKGROUND, NULL, true,
		CharacterStatus(1, false, 0, 0)
	};

	// WRÓG
	factory_status[GHOST] = {
		Character::ENEMY, Body::NONE, new SnailAI(NULL, 2.f), true,
		CharacterStatus(-1, false, 0, 0)
	};

	// KOLCE
	factory_status[SPIKES] = {
		Character::SPIKES, Body::NONE, NULL, true,
		CharacterStatus(-1, false, 0, 0)
	};

	// POCISK
	factory_status[BULLET] = {
		Character::SPIKES, Body::NONE, NULL, true,
		CharacterStatus(-1, false, 0, 0)
	};

	// DRABINA
	factory_status[LADDER] = {
		Character::LADDER, Body::BACKGROUND, NULL, false
	};

	// LIANA
	factory_status[LIANE] = {
		Character::LIANE, Body::BACKGROUND, NULL, false
	};
}

/**
 * Podstawowe tekstury fabryki
 */
ResourceFactory::_TextureConfig ResourceFactory::factory_types[] =
		{
		// KOLCE
			{
				SPIKES,
				pEngine::RIGHT,
				90.f,
				20,
				"kolce.txt",
				"spikes_right",
				false },
			{
				SPIKES,
				pEngine::LEFT,
				-90.f,
				20,
				"kolce.txt",
				"spikes_left",
				false },
			{ SPIKES, pEngine::UP, 0.f, 23, "kolce.txt", "spikes_up", false },
			{
				SPIKES,
				pEngine::DOWN,
				180.f,
				23,
				"kolce.txt",
				"spikes_down",
				false },

			// PUNKTY
			{ SCORE, pEngine::NONE, 0.f, 12, "punkt.txt", "score", false },
			{ HEALTH, pEngine::NONE, 0.f, 16, "zycie.txt", "health", false },
			{ GHOST, pEngine::NONE, 0.f, 12, "wrog.txt", "enemy", false },

			// BRONIE
			{ GUN, pEngine::RIGHT, 90.f, 12, "bron.txt", "gun_right", false },
			{ GUN, pEngine::LEFT, -90.f, 12, "bron.txt", "gun_left", false },
			{ GUN, pEngine::UP, 0.f, 16, "bron.txt", "gun_up", false },
			{ GUN, pEngine::DOWN, 180.f, 16, "bron.txt", "gun_down", false },

			// DRABINKI
			{ LADDER, pEngine::NONE, 0.f, 23, "drabina.txt", "stairs", false },
			{ LIANE, pEngine::NONE, 0.f, 16, "liana.txt", "liane", true },

			// POCISKI
			{
				BULLET,
				pEngine::RIGHT,
				90.f,
				0,
				"pocisk.txt",
				"bullet_right",
				false },
			{
				BULLET,
				pEngine::LEFT,
				-90.f,
				0,
				"pocisk.txt",
				"bullet_left",
				false },
			{ BULLET, pEngine::UP, 0.f, 0, "pocisk.txt", "bullet_up", false },
			{
				BULLET,
				pEngine::DOWN,
				180.f,
				0,
				"pocisk.txt",
				"bullet_down",
				false } };

/**
 * Konstruktor prywatny!
 */
ResourceFactory::ResourceFactory() :
				//
				texture_temperature(NORMAL),
				physics(NULL) {
	generateChracterStatus(EASY);
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

/**
 * Wczytywanie tekstur!
 */
void ResourceFactory::loadMainTexturesPack() {
	// Gracz
	readShape("gracz_mikolaj.txt", "player", 0);
	readShape("czaszka.txt", "cranium", 0);
	
	// Moby
	loadMobsTexturesPack("", false);
	changeTemperatureOfTextures(ICY);

	//
	logEvent(Logger::LOG_INFO, "Pomyślnie wczytano paczkę tekstur!");
}

void ResourceFactory::loadMobsTexturesPack(const char* _addition,
		bool _temperature_changed) {
	// Tekstury mobów
	for (_TextureConfig& factory_object : factory_types) {
		if (_temperature_changed && !factory_object.temperature_enabled) {
			continue;
		}
		usint _resource_id = genTextureID(
				factory_object.type,
				factory_object.orientation);
		factory_object.resource_id = _resource_id;
		//
		putTexture(
				_resource_id,
				readShape(
						((string) factory_object.file_name + _addition).c_str(),
						factory_object.resource_label,
						factory_object.rotation));
	}
}

/**
 * Zmiana temperatur tekstur!
 */
void ResourceFactory::changeTemperatureOfTextures(usint _texure_temperature) {
	texture_temperature = _texure_temperature;

	// Dodatek do wczytywanej mapy
	const char* addition;
	switch (texture_temperature) {
		//
		case ICY:
			addition = "_zima";
			break;

			//
		case NEUTRAL:
			addition = NULL;
			break;

			//
		case HOT:
			addition = "_lato";
			break;

	}

	// Usuwanie starych tekstur
	for (_TextureConfig& factory_object : factory_types) {
		if (factory_object.temperature_enabled) {
			main_resource_manager.deleteResource(factory_object.resource_id);
		}
	}

	// Wczytywanie na nowo
	loadMobsTexturesPack(addition, true);
}

/**
 * Dokładanie tekstur!
 */
void ResourceFactory::putTexture(usint id, PlatformShape* shape) {
	textures[id] = shape;
}

ResourceFactory& ResourceFactory::getIstance(pEngine* _physics) {
	static ResourceFactory factory;
	if (_physics) {
		if (!factory.physics) {
			factory.loadMainTexturesPack();
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
	 * Przeszukiwanie bazy obiektów
	 */
	_TextureConfig* _factory_type = getFactoryType(_type, _orientation);
	usint _width = _factory_type ? _factory_type->width : 0;
	Platform* _object = NULL;

	if (_type == KILLZONE) {
		/**
		 * STREFA ŚMIERCI
		 */
		_object = new Platform(_x, _y, _w, _h, oglWrapper::WHITE, Body::HIDDEN);
	} else if (_type == GUN) {
		/**
		 * BROŃ
		 */
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
	} else {
		/**
		 * INNE OBIEKTY
		 */
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
		 * Generowanie obiektów
		 */
		if (_type == OBJECT) {
			character->setType(Character::PLATFORM);
			character->fitToWidth(_w);
		} else {
			/**
			 * Pobieranie statusu BUG!
			 */
			_FactoryStatus _status = factory_status[_type];

			character->setType(_status.character_type);
			character->setState(_status.state);
			if (_status.ai) {
				character->setAI(_status.ai);
			}
			if (_status.is_score) {
				character->setStatus(_status.character_status);
			}
		}
	}
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
