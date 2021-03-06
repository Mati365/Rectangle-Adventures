/*
 * MapINFO.cpp
 *
 *  Created on: 18-04-2013
 *      Author: mateusz
 */
#include "Resources.hpp"

#include "../../Gameplay/Objects/Objects.hpp"

#include "../../Tools/Converter.hpp"
#include "../../Tools/Logger.hpp"

///////////////////////////////////////

bool loadMap(const char* path, MapINFO* map, usint _open_config) {
	if (!map) {
		return false;
	}
	FILE* __map = nullptr;
	
#ifdef FILESYSTEM_USAGE
	__map = main_filesystem.getExternalFile(path, nullptr);
#else
	__map = fopen(("mobs/" + (string) path).c_str(), "r");
#endif
	
	map->open_config = _open_config;
	if (!map->load(__map)) {
		return false;
	}
	main_filesystem.closeExternalFile();
	
	return true;
}

MapINFO* loadMap(const char* path, usint _open_config) {
	MapINFO* map = new MapINFO(path);
	if (!loadMap(path, map, _open_config)) {
		safe_delete<MapINFO>(map);
	}
	return map;
}

/** Wczytywanie moba */
Body* readMob(FILE* file) {
	usint type;
	usint orientation;
	usint script_id;
	usint state;
	Vector<float> pos;
	
	//
	fscanf(
			file,
			"%hu %f %f %hu %hu %hu\n",
			&type,
			&pos.x,
			&pos.y,
			&orientation,
			&script_id,
			&state);
	//
	Body* body = ResourceFactory::getInstance(nullptr).createObject(
			type,
			pos.x,
			pos.y,
			0,
			0,
			nullptr,
			nullptr,
			orientation,
			state);
	//
	body->script_id = script_id;
	return body;
}

/**
 * Blad projektowy:
 * + MUSI ISTNIEC TYLKO 1 AKTYWNY OBIEKT
 * BO WCZYTYWANE SA NOWE KOMPLETY TEKSTUR
 * MOZE KIEDYS SIE TO NAPRAWI ;)
 */

MapINFO::MapINFO(const char* _label) :
				Resource(_label),
				open_config(0),
				physics(NULL),
				hero_bounds(0, 0),
				hero_shape(NULL),
				map_weather(0),
				map_temperature(1) {
}

/**
 * Wczytywanie headeru mapy:
 * + temperatury
 * + pogody
 * + info o graczu
 */
void MapINFO::readHeader(FILE* map) {
	char shape[256];
	
	/** Wczytywanie ustawien mapy */
	fscanf(map, "%hu %hu", &map_temperature, &map_weather);
	
	/** Przestrzeganie konfiguracji! */
	if (IS_SET(open_config, WITHOUT_WEATHER)) {
		map_weather = 0;
	}
	
	if (IS_SET(open_config, WITHOUT_TEMPERATURE)) {
		map_temperature = ResourceFactory::TextureTemperature::NEUTRAL;
	}
	
	/** Wczytywanie pozycji poczatkowej gracza */
	fscanf(
			map,
			"%f %f %f %s\n",
			&hero_bounds.x,
			&hero_bounds.y,
			&hero_bounds.w,
			shape);
	
	if (!IS_SET(open_config, WITHOUT_HERO) && strlen(shape) != 0) {
		hero_shape = readShape(shape, shape, 0);
		if (hero_shape) {
			resources.push_back( { hero_shape->getResourceID(), shape });
		} else {
			logEvent(Logger::LOG_WARNING, "Brak tekstury gracza");
		}
	}
	//
	PROGRESS_LOADING();
}

/**
 * Wczytywanie listy ksztaltow:
 * + Wczytywanie do resources
 * + w funkcji unload wywalane!
 */
void MapINFO::readShapes(FILE* map) {
	char path[256];
	usint size;
	
	/** Wczytywanie listy ksztaltow */
	fscanf(map, "%hu\n", &size);
	for (usint i = 0; i < size; ++i) {
		fscanf(map, "%s\n", path);
		
		/** Domyslny kat to 0* */
		PlatformShape* _shape = readShape(path, path, 0);
		_shape->setLineWidth(3.f); // lepiej wyglada
				
		resources.push_back( { _shape->getResourceID(), path });
	}
	//
	PROGRESS_LOADING();
}

/**
 * Wczytywanie listy platform
 * + Usuwane w funkcji unload
 * + Korzystanie z tekstur z resources
 */
void MapINFO::readPlatforms(FILE* map) {
	Rect<float> rect;
	Color col;
	usint size = 0, state = 0, layer = 0;
	
	/** Poruszanie sie platformy */
	Vector<float> max_distance;
	Vector<float> velocity;
	usint repeat_movement;
	
	/** Ksztalt */
	usint fill_type;
	usint with_shape;
	usint script_id;
	
	char shape[256];
	usint border[4];
	
	deque<Body*> objects;
	
	/** Wczytywanie platform */
	fscanf(map, "%hu\n", &size);
	
	for (usint i = 0; i < size; ++i) {
		fscanf(
				map,
				"%hu %hu %hu %hu %hu %hu %hu %f %f %f %f %hu %f %f %f %f %hu %hu %hu %hu %hu %hu %s\n",
				&script_id,
				&border[0],
				&border[1],
				&border[2],
				&border[3],
				&fill_type,
				&repeat_movement,
				&max_distance.x,
				&max_distance.y,
				&velocity.x,
				&velocity.y,
				&state,
				&rect.x,
				&rect.y,
				&rect.w,
				&rect.h,
				&col.r,
				&col.g,
				&col.b,
				&col.a,
				&layer,
				&with_shape,
				shape);
		//
		Platform* platform = nullptr;
		
		/** Wczytywanie ksztaltu */
		if (with_shape) {
			/** Wyszukiwanie nie powtarzajacego sie identyfikatora textury! */
			usint _resource_id = 0;
			for (auto& res : resources) {
				if (strcmp(res.label, shape) == 0) {
					_resource_id = res.id;
				}
			}
			
			/** Tworzenie platformy niereguralnej */
			platform = new IrregularPlatform(rect.x, rect.y, state,
			// Bugfix: Stara mapa w pamieci moze miec ta sama teksture!
					dynamic_cast<PlatformShape*>(main_resource_manager.getByID(
							_resource_id)));
			
			IrregularPlatform* __platform =
					dynamic_cast<IrregularPlatform*>(platform);
			
			__platform->fitToWidth(rect.w);
		} else {
			/** Platforma nrmalna */
			platform = new Platform(rect.x, rect.y, rect.w, rect.h, col, state);
			
			platform->setBorder(border[0], border[1], border[2], border[3]);
			platform->setFillType(fill_type);
		}
		
		/** Warstwa obiektu */
		platform->layer = layer;
		platform->script_id = script_id;
		
		/**
		 * Kierunek ruchu
		 */
		if (velocity.x != 0 || velocity.y != 0) {
			platform->setMovingDir(velocity, max_distance, repeat_movement);
		}
		platform->compileList();
		objects.push_back(platform);
	}
	PROGRESS_LOADING();

	/** Wyliczanie wymiarow planszy */
	Rect<float> max;
	for (auto& obj : objects) {
		float _x = obj->x + obj->w;
		float _y = obj->y + obj->h;
		if (max.w < _x) {
			max.w = _x;
		}
		if (max.h < _y) {
			max.h = _y;
		}
	}
	bounds = max;
	//
	PROGRESS_LOADING();

	/** Dodawanie elementow */
	safe_delete<pEngine>(physics);
	physics = new pEngine(bounds, 0.6f);
	
	for (auto& obj : objects) {
		physics->insert(obj);
	}
	//
	PROGRESS_LOADING();
}

/**
 * Wczytywanie mobow i skryptow:
 * + Korzystaja z fabryki
 */
void MapINFO::readMobsAndTriggers(FILE* map) {
	char shape[256];
	usint size;
	Rect<float> rect(0, 0, 0, 0);
	
	/** Wczytywanie mobow! */
	usint _max_score = 0;

	fscanf(map, "%hu\n", &size);
	for (usint i = 0; i < size; ++i) {
		Character* _body = dynamic_cast<Character*>(readMob(map));
		if (_body && _body->getStatus()->score > 0) {
			_max_score += _body->getStatus()->score;
		}
	}
	if(_max_score > 1) {
		max_score = _max_score;
	}
	
	/** Wczytywanie skryptow! */
	fscanf(map, "%hu\n", &size);
	for (usint i = 0; i < size; ++i) {
		memset(shape, 0, 256 * sizeof(char));
		fscanf(
				map,
				"%f %f %f %f %[^\n]s\n",
				&rect.x,
				&rect.y,
				&rect.w,
				&rect.h,
				shape);
		//
		ResourceFactory::getInstance(nullptr).createObject(
				ResourceFactory::SCRIPT_BOX,
				rect.x,
				rect.y,
				rect.w,
				rect.h,
				nullptr,
				shape,
				pEngine::NONE);
	}
	//
	PROGRESS_LOADING();
}

/**
 *  Opis zapisu platformy:
 *  [obramowanie] [typ zamalowania] [petla ruchu 0:1] [to_x] [to_y] [speed_x] [speed_y] [flag] [4 kolory] [level] [ksztalt 0:1] [nazwa ksztaltu]
 *
 *  Opis zapisu moba:
 *  [typ] [x] [y] [dodatkowy argument np. tekst do intro]
 */
bool MapINFO::load(FILE* map) {
	if (!map) {
		return false;
	}
	BEGIN_LOADING("MAP extracting:");
	//
	ResourceFactory::getInstance(NULL).unload();
	
	readHeader(map);
	readShapes(map);
	readPlatforms(map);
	
	/** Inicjacja fizyki! */
	ResourceFactory::getInstance(physics);
	
	readMobsAndTriggers(map);
	//
	END_LOADING();
	return true;
}

/** Usuwanie wszystkiego co wczytane! */
void MapINFO::unload() {
	/** Usuwanie ksztaltow wolneeee! */
	for (auto& res_id : resources) {
		if (!main_resource_manager.deleteResource(res_id.id)) {
			logEvent(
					Logger::LOG_WARNING,
					"BUG! Nie moge skasowac ksztaltu! Mozliwy mem. leak!");
		}
	}
	ResourceFactory::getInstance(NULL).unload();
	
	/** Kasowanie fizyki razem z obiektami! */
	safe_delete<pEngine>(physics);
	
	logEvent(Logger::LOG_INFO, "Wywalilem mape!");
}
