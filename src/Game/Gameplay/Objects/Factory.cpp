/*
 * Factory.cpp
 *
 *  Created on: 30-05-2013
 *      Author: mateusz
 */
#include "Objects.hpp"
#include "Weapons.hpp"

#include "../../Tools/Logger.hpp"

/** Generowanie statusow */
map<usint, ResourceFactory::_FactoryStatus> ResourceFactory::factory_status;

void ResourceFactory::generateChracterStatus(usint _hard_level) {
	factory_status.clear();
	
	// PUNKT
	factory_status[SCORE] = {
		Character::SCORE, Body::BACKGROUND, true,
		CharacterStatus(0, false, 0, 1)
	};

	// ZYCIE
	factory_status[HEALTH] = {
		Character::SCORE, Body::BACKGROUND, true,
		CharacterStatus(1, false, 0, 0)
	};

	// WROG
	factory_status[GHOST] = {
		Character::ENEMY, Body::NONE, true,
		CharacterStatus(-1, false, 0, 0)
	};

	// KOLCE
	factory_status[SPIKES] = {
		Character::SPIKES, Body::NONE, true,
		CharacterStatus(-1, false, 0, 0)
	};

	// POCISK
	factory_status[BULLET] = {
		Character::SPIKES, Body::NONE, true,
		CharacterStatus(-1, false, 0, 0)
	};

	// DRABINA
	factory_status[LADDER] = {
		Character::LADDER, Body::BACKGROUND, false
	};

	// LIANA
	factory_status[LIANE] = {
		Character::LIANE, Body::BACKGROUND, false
	};
}

/** Podstawowe tekstury w fabryce */
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
				true },
			{ SPIKES, pEngine::LEFT, -90.f, 20, "kolce.txt", "spikes_left", true },
			{ SPIKES, pEngine::UP, 0.f, 23, "kolce.txt", "spikes_up", true },
			{ SPIKES, pEngine::DOWN, 180.f, 23, "kolce.txt", "spikes_down", true },
			
			// PUNKTY
			{ SCORE, pEngine::NONE, 0.f, 12, "punkt.txt", "score", true },
			{ HEALTH, pEngine::NONE, 0.f, 16, "zycie.txt", "health", false },
			{ GHOST, pEngine::NONE, 0.f, 18, "wrog.txt", "enemy", false },
			
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
				true },
			{ BULLET, pEngine::LEFT, -90.f, 0, "pocisk.txt", "bullet_left", true },
			{ BULLET, pEngine::UP, 0.f, 0, "pocisk.txt", "bullet_up", true },
			{ BULLET, pEngine::DOWN, 180.f, 0, "pocisk.txt", "bullet_down", true } };

/** Konstruktor */
ResourceFactory::ResourceFactory() :
				//
				texture_temperature(NORMAL),
				physics(NULL) {
	generateChracterStatus(EASY);
}

/**
 * Mapowanie podstawowych tekstur w grze!
 *
 * UWAGA! KOLEJNE KIERUNKI TEKSTUR SA FLAGAMI +MAX_ILOSC od
 * PIERWOTNEJ!!!
 */
#define MAX_TEXTURES_COUNT 300

/** Generowanie kolejnych id tekstur dla poszczegolnych orientacji */
usint ResourceFactory::genTextureID(usint _factory_type, usint _orientation) {
	return 5 * _factory_type + MAX_TEXTURES_COUNT + _orientation;
}

/** Wczytywanie tekstur! */
void ResourceFactory::loadMainTexturesPack() {
	// Gracz
	readShape("czaszka.txt", "cranium", 0);
	
	// Ikonki
	readShape("hud_od_nowa.txt", "retry_shape", 0.f);
	
	// Moby
	loadMobsTexturesPack("");
	
	logEvent(Logger::LOG_INFO, "Pomyslnie wczytano paczkae tekstur!");
}

void ResourceFactory::loadMobsTexturesPack(const char* _addition) {
	/** Tekstury mobow */
	usint _total_deleted = 0;
	bool _first_load = textures.empty();
	
	for (_TextureConfig& factory_object : factory_types) {
		if (!_first_load && !factory_object.temperature_enabled) {
			continue;
		}
		usint _texture_id = genTextureID(
				factory_object.factory_type,
				factory_object.orientation);
		
		/** Usuwanie starej tekstury */
		if (!_first_load) {
			if (!main_resource_manager.deleteResource(
					factory_object.resource_id)) {
				logEvent(
						Logger::LOG_WARNING,
						"Bug: Problem ze skasowaniem obiektu!");
			}
			textures.erase(_texture_id);
		}
		
		/** Wczytywanie nowych tekstur */
		string filename = factory_object.file_name;
		if (_addition) {
			filename += _addition;
		}
		PlatformShape* _new_shape = readShape(
				filename,
				factory_object.resource_label,
				factory_object.rotation);
		putTexture(_texture_id, _new_shape);
		
		/** Bugfix! ResourceID != TextureID */
		factory_object.resource_id = _new_shape->getResourceID();
		
		/** Liczenie skasowanych */
		_total_deleted++;
	}
	logEvent(
			Logger::LOG_INFO,
			"Zarejestrowano " + Convert::toString<usint>(_total_deleted)
					+ " obiektĂłw! Brak memoryleak!");
}

/**
 * Reallokacja tekstur, obiekty posiadaja… stare
 * wskazniki do tekstur!
 */
bool ResourceFactory::texturePackRealloc() {
	if (created.empty()) {
		return false;
	}
	usint reallocated = 0;
	
	for (usint i = 0; i < created.size(); ++i) {
		Body* obj = created[i];
		
		/** Odbiorca tekstury */
		if (!obj) {
			created.erase(created.begin() + i);
			continue;
		}
		
		/** Konfiguracja tekstury */
		_TextureConfig* tex_conf = getFactoryTemplate(
				obj->factory_type,
				obj->orientation);
		if (!tex_conf) {
			continue;
		}
		
		IrregularPlatform* receiver = dynamic_cast<IrregularPlatform*>(obj);
		if (!receiver) {
			logEvent(Logger::LOG_ERROR, "BUG: Brak odbiorcy tekstury!");
			continue;
		}
		
		/** Nowa tekstura */
		PlatformShape* new_shape =
				dynamic_cast<PlatformShape*>(main_resource_manager.getByID(
						tex_conf->resource_id));
		
		if (!new_shape) {
			logEvent(
					Logger::LOG_WARNING,
					(string )"BUG: Pusta przealokowana tekstura! ");
			continue;
		}
		receiver->setShape(new_shape);
		receiver->fitToWidth(tex_conf->width);
		
		/**
		 * Oprucz samej tekstury dzialka
		 * musza… zostac przeladowane tekstury
		 * broni na nim
		 */
		if (obj->factory_type == GUN) {
			Gun* gun = dynamic_cast<Gun*>(obj);
			gun->setBulletsShape( {
									textures[genTextureID(BULLET, 1)],
									textures[genTextureID(BULLET, 2)],
									textures[genTextureID(BULLET, 3)],
									textures[genTextureID(BULLET, 4)] });
		}
		
		/** Liczenie */
		reallocated++;
	}
	
	logEvent(
			Logger::LOG_INFO,
			"Przealokowano " + Convert::toString<usint>(reallocated)
					+ " obiektĂłw!");
	return true;
}

/** Pobieranie tekstur! po id i orientacji */
PlatformShape* ResourceFactory::getTexture(usint _id, usint _orientation) {
	return textures[genTextureID(_id, _orientation)];
}

/** Zmiana temperatur tekstur! */
void ResourceFactory::changeTemperatureOfTextures(usint _texure_temperature) {
	/**
	 * Po co wczytywac 2 razy te same obiekty?
	 * Mniejsze ryzyko wycieku pamieci
	 */
	if (_texure_temperature == texture_temperature) {
		return;
	}
	texture_temperature = _texure_temperature;
	
	/** Dodatek do wczytywanej mapy */
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
	
	/** Wczytywanie na nowo */
	loadMobsTexturesPack(addition);
	texturePackRealloc();
}

/** Dokladanie tekstur */
void ResourceFactory::putTexture(usint id, PlatformShape* shape) {
	textures[id] = shape;
}

ResourceFactory& ResourceFactory::getInstance(pEngine* _physics) {
	static ResourceFactory factory;
	if (_physics) {
		if (!factory.physics) {
			factory.loadMainTexturesPack();
		}
		factory.physics = _physics;
	}
	return factory;
}

/** Generowanie obiektow */
Body* ResourceFactory::createObject(usint _type, float _x, float _y, float _w,
		float _h, PlatformShape* _shape, char* _script, usint _orientation,
		usint _state) {
	if (physics == NULL) {
		logEvent(Logger::LOG_ERROR, "Fabryka zgĹ‚asza praw fizyki brak!");
		return NULL;
	}
	
	/** Identyfikator tekstury obikektu! */
	usint _texture_id = genTextureID(_type, _orientation);
	
	/**
	 * Obiekt skryptu i portalu dziedziczy tylko
	 * od Body!
	 */
	if (_type == PORTAL_BEGIN || _type == PORTAL_END) {
		Portal* portal = new Portal(_x, _y, _orientation, Body::FLYING);
		if (_type == PORTAL_END) {
			if (created.empty()) {
				safe_delete<Portal>(portal);
				return nullptr;
			}
			portal->linkTo(dynamic_cast<Portal*>(created.back()));
		}
		
		addBody(portal);
		//
		return portal;
	}
	
	if (_type == SCRIPT_BOX) {
		if (!_script) {
			logEvent(Logger::LOG_WARNING, "Nie mogÄ™ utworzyÄ‡ triggera!");
			return NULL;
		}
		Trigger* trigger = new Trigger(
				Interpreter::getIstance().compile(_script),
				_x,
				_y,
				_w,
				_h);
		physics->insert(trigger);
		//
		return trigger;
	}
	
	/** Przeszukiwanie bazy obiektow */
	_TextureConfig* _factory_type = getFactoryTemplate(_type, _orientation);
	usint _width = _factory_type ? _factory_type->width : 0;
	Platform* _object = NULL;
	
	if (_type == KILLZONE) {
		/** Strefa smierci */
		_object = new Platform(_x, _y, _w, _h, oglWrapper::WHITE, Body::HIDDEN);
		
	} else if (_type == GUN) {
		/** Bron */
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
				80);
		dynamic_cast<Gun*>(_object)->fitToWidth(_width);
	} else {
		/** Inne obiekty */
		_object = new Character(
				"",
				_x,
				_y,
				_shape == NULL ? textures[_texture_id] : _shape,
				Character::NONE);
		Character* character = dynamic_cast<Character*>(_object);
		character->fitToWidth(_width);
		
		if (_type == OBJECT) {
			/** Generowanie pustej platformy */
			character->setType(Character::PLATFORM);
			character->fitToWidth(_w);
		} else {
			/** Pobieranie statusu */
			_FactoryStatus _status = factory_status[_type];
			
			character->setType(_status.character_type);
			character->setState(_status.state);
			if (_status.is_score) {
				character->setStatus(_status.character_status);
			}
		}
	}
	
	/** Potrzebne przy realokacji tekstur */
	_object->orientation = _orientation;
	_object->factory_type = _type;
	if (_state != Body::NONE) {
		_object->state = _state;
	}
	
	addBody(_object);
	//
	return _object;
}

/** Usuwanie resztek i reset */
void ResourceFactory::unload() {
	created.clear();
}

/** Rejestrowanie nowego obiektu */
void ResourceFactory::addBody(Body* _object) {
	if (_object) {
		created.push_back(_object);
		
		/** Silowe dodanie do listy! */
		physics->insert(_object);
	}
}
