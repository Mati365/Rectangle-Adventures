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
	FILE* __map = main_filesystem.getExternalFile(path, NULL);
	
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

/**
 * Wczytywanie mob'a
 */
bool readMob(FILE* file) {
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
	Body* body = ResourceFactory::getInstance(NULL).createObject(
			type,
			pos.x,
			pos.y,
			0,
			0,
			NULL,
			NULL,
			orientation,
			state);
	//
	body->script_id = script_id;
	return true;
}

/**
 * Błąd projektowy:
 * + MUSI ISTNIEĆ TYLKO 1 AKTYWNY OBIEKT
 * BO WCZYTYWANE SĄ NOWE KOMPLETY TEKSTUR
 * Może kiedyś się to naprawi ;)
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
	
	/** Wczytywanie ustawień mapy */
	fscanf(map, "%hu %hu", &map_temperature, &map_weather);
	
	/** Przestrzeganie konfiguracji! */
	if (IS_SET(open_config, WITHOUT_WEATHER)) {
		map_weather = 0;
	}
	
	if (IS_SET(open_config, WITHOUT_TEMPERATURE)) {
		map_temperature = ResourceFactory::TextureTemperature::NEUTRAL;
	}
	
	/** Wczytywanie pozycji początkowej gracza */
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
 * Wczytywanie listy kształtów:
 * + Wczytywanie do resources
 * + w funkcji unload wywalane!
 */
void MapINFO::readShapes(FILE* map) {
	char path[256];
	usint size;
	
	/** Wczytywanie listy kształtów */
	fscanf(map, "%hu\n", &size);
	for (usint i = 0; i < size; ++i) {
		fscanf(map, "%s\n", path);
		
		/** Domyślny kąt to 0* */
		PlatformShape* _shape = readShape(path, path, 0);
		_shape->setLineWidth(3.f); // lepiej wygląda
				
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
	
	/**
	 * Poruszanie się platformy
	 */
	Vector<float> max_distance;
	Vector<float> velocity;
	usint repeat_movement;
	/**
	 * Kształt
	 */
	usint fill_type;
	usint with_shape;
	usint script_id;
	
	char shape[256];
	usint border[4];
	
	deque<Body*> objects;
	
	cout << endl;
	for(auto& res : resources) {
		cout << res.label << endl;
	}

	// Wczytywanie parametrów graficznych platform..
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
		Platform* platform = NULL;
		
		/**
		 * Wczytywanie kształtu
		 */
		if (with_shape) {
			/**
			 * Wyszukiwanie nie powtarzającego się identyfikatora
			 * tekstury!
			 */
			usint _resource_id = 0;
			for (auto& res : resources) {
				if (strcmp(res.label, shape) == 0) {
					_resource_id = res.id;
				}
			}

			/**
			 * Platforma niereguralna - kształt
			 */
			platform = new IrregularPlatform(rect.x, rect.y, state,
			// Bugfix: Stara mapa w pamięci może mieć tą samą teksturę!
					dynamic_cast<PlatformShape*>(main_resource_manager.getByID(
							_resource_id)));

			IrregularPlatform* __platform =
					dynamic_cast<IrregularPlatform*>(platform);

			__platform->fitToWidth(rect.w);
		} else {
			/**
			 * Normalna platforma
			 */
			platform = new Platform(rect.x, rect.y, rect.w, rect.h, col, state);

			platform->setBorder(border[0], border[1], border[2], border[3]);
			platform->setFillType(fill_type);
		}
		
		/**
		 * Warstwa obiektu
		 */
		platform->layer = layer;
		platform->script_id = script_id;
		
		/**
		 * Kierunek ruchu
		 */
		if (velocity.x != 0 || velocity.y != 0) {
			platform->setMovingDir(velocity, max_distance, repeat_movement);
		}
		platform->compileList();
//
		objects.push_back(platform);
	}
	PROGRESS_LOADING();
//

	/**
	 * Wyliczanie wymiarów planszy
	 */
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

	/**
	 * Dodawanie elementów
	 */
	safe_delete<pEngine>(physics);
	physics = new pEngine(bounds, 0.2f);

	for (auto& obj : objects) {
		physics->insert(obj);
	}
//
	PROGRESS_LOADING();
}

/**
 * Wczytywanie mobów i skryptów:
 * + Korzystają z fabryki
 */
void MapINFO::readMobsAndTriggers(FILE* map) {
	char shape[256];
	usint size;
	Rect<float> rect(0, 0, 0, 0);

	/**
	 * Wczytywanie mobów!
	 */
	fscanf(map, "%hu\n", &size);
	for (usint i = 0; i < size; ++i) {
		readMob(map);
	}

	/**
	 * Wczytywanie skryptów!
	 */
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
		ResourceFactory::getInstance(NULL).createObject(
				ResourceFactory::SCRIPT_BOX,
				rect.x,
				rect.y,
				rect.w,
				rect.h,
				NULL,
				shape,
				pEngine::NONE);
	}
//
	PROGRESS_LOADING();
}

/**
 *  Opis zapisu platformy:
 *  [obramowanie] [typ zamalowania] [pętla ruchu 0:1] [to_x] [to_y] [speed_x] [speed_y] [flag] [4 kolory] [level] [ksztalt 0:1] [nazwa ksztaltu]
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
	
	// Inicjacja fizyki!
	ResourceFactory::getInstance(physics);
	
	readMobsAndTriggers(map);
	//
	END_LOADING();
	return true;
}

/**
 * Usuwanie wszystkiego co wczytane!
 */
void MapINFO::unload() {
	// Usuwanie kształtów! Woolne!
	for (auto& res_id : resources) {
		if (!main_resource_manager.deleteResource(res_id.id)) {
			logEvent(
					Logger::LOG_WARNING,
					"BUG! Nie mogę skasować zasobu - możliwy memoryleak!");
		}
	}
	ResourceFactory::getInstance(NULL).unload();
	
	// Kasowanie fizyki razem z obiektami!
	safe_delete<pEngine>(physics);
	
	logEvent(Logger::LOG_INFO, "Pomyślnie usunięto mapę!");
}
