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

bool loadMap(const char* path, MapINFO* map) {
	FILE* __map = main_filesystem.getExternalFile(path, NULL);
	if (!map->load(__map)) {
		return false;
	}
	main_filesystem.closeExternalFile();
	return true;
}

MapINFO* loadMap(const char* path) {
	MapINFO* map = new MapINFO(path);
	if (!loadMap(path, map)) {
		delete map;
		//
		return NULL;
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
			orientation);
	//
	body->script_id = script_id;
	if (body->state == 1) { // NONE = 1
		body->state = state;
	}
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
				physics(NULL),
				hero_bounds(0, 0),
				hero_shape(NULL),
				map_weather(0),
				map_temperature(1) {
}

/**
 * Wyliczanie maksymalnych wymiarów mapy
 * z platform statycznych, nie mobów!
 */
void MapINFO::calcBounds() {
	Rect<float> max;
	for (usint i = 0; i < platforms.size(); ++i) {
		float _x = platforms[i]->x + platforms[i]->w;
		float _y = platforms[i]->y + platforms[i]->h;
		if (max.w < _x) {
			max.w = _x;
		}
		if (max.h < _y) {
			max.h = _y;
		}
	}
	bounds = max;
}

/**
 * Wczytywanie headeru mapy:
 * + temperatury
 * + pogody
 * + info o graczu
 */
void MapINFO::readHeader(FILE* map) {
	char shape[256];

	// Wczytywanie ustawień mapy
	fscanf(map, "%hu %hu", &map_temperature, &map_weather);

	// Wczytywanie pozycji początkowej gracza
	fscanf(
			map,
			"%f %f %f %s\n",
			&hero_bounds.x,
			&hero_bounds.y,
			&hero_bounds.w,
			shape);

	if (strlen(shape) != 0) {
		hero_shape = readShape(shape, shape, 0);
		if (hero_shape) {
			resources.push_back(hero_shape->getResourceID());
		} else {
			logEvent(Logger::LOG_WARNING, "Brak tekstury gracza");
		}
	}
}

/**
 * Wczytywanie listy kształtów:
 * + Wczytywanie do resources
 * + w funkcji unload wywalane!
 */
void MapINFO::readShapes(FILE* map) {
	char shape[256];
	usint size;

	// Wczytywanie listy kształtów
	fscanf(map, "%hu\n", &size);
	for (usint i = 0; i < size; ++i) {
		fscanf(map, "%s\n", shape);

		// Domyślny kąt to 0*
		resources.push_back(readShape(shape, shape, 0)->getResourceID());
	}
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
			 * Platforma niereguralna - kształt
			 */
			platform =
					new IrregularPlatform(
							rect.x,
							rect.y,
							state,
							dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(
									shape)));
			dynamic_cast<IrregularPlatform*>(platform)->fitToWidth(rect.w);
		} else {
			/**
			 * Normalna platforma
			 */
			platform = new Platform(rect.x, rect.y, rect.w, rect.h, col, state);
			platform->setBorder(border[0], border[1], border[2], border[3]);
		}
		platform->setFillType(fill_type);

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
		platforms.push_back(platform);
	}
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
		ResourceFactory::getInstance(physics).createObject(
				ResourceFactory::SCRIPT_BOX,
				rect.x,
				rect.y,
				rect.w,
				rect.h,
				NULL,
				shape,
				pEngine::NONE);
	}
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

	readHeader(map);
	readShapes(map);
	readPlatforms(map);
	
	// Obliczanie wymiarów!
	calcBounds();
	
	// Inicjacja fizyki!
	if (physics) {
		delete physics;
	}
	physics = new pEngine(bounds, 0.3f);
	for (auto iter = platforms.begin(); iter != platforms.end(); ++iter) {
		physics->insert(*iter);
	}
	ResourceFactory::getInstance(physics);

	readMobsAndTriggers(map);
	return true;
}

/**
 * Usuwanie wszystkiego co wczytane!
 */
void MapINFO::unload() {
	if (physics) {
		delete physics;
	}
	/**
	 * Szybsze niż AllocKiller
	 */
	for (auto* obj : platforms) {
		delete obj;
	}
	ResourceFactory::getInstance(NULL).unloadObjects();

	// Usuwanie kształtów! Woolne!
	for (auto& res_id : resources) {
		if (!main_resource_manager.deleteResource(res_id)) {
			logEvent(
					Logger::LOG_WARNING,
					"BUG! Nie mogę skasować zasobu - możliwy memoryleak!");
		}
	}
	logEvent(Logger::LOG_INFO, "Pomyślnie usunięto mapę!");
}
