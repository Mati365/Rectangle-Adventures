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
		return NULL;
	}
	return map;
}

bool readMob(FILE* file) {
	usint type;
	Vector<float> pos;
	char shape[255];
	
	fscanf(file, "%hu %f %f %s\n", &type, &pos.x, &pos.y, shape);
	//
	ResourceFactory::getIstance(NULL).createObject(
			(ResourceFactory::Types) type, pos.x, pos.y, 0, 0, NULL, NULL);
	return true;
}

///////////////////////////////////////

MapINFO::MapINFO(const char* _label) :
				Resource<usint>(_label),
				physics(NULL),
				hero_bounds(0, 0) {
}

void MapINFO::calcBounds() {
	Rect<float> max;
	for (usint i = 0; i < objects.size(); ++i) {
		float _x = objects[i]->x + objects[i]->w;
		float _y = objects[i]->y + objects[i]->h;
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
	usint type;
	usint with_shape;
	usint script_id;
	
	char shape[256];
	int border[4];
	
	// Wczytywanie pozycji początkowej gracza
	fscanf(map, "%f %f %f\n", &hero_bounds.x, &hero_bounds.y, &hero_bounds.w);
	
	// Wczytywanie listy kształtów
	fscanf(map, "%hu\n", &size);
	for (usint i = 0; i < size; ++i) {
		fscanf(map, "%s\n", shape);
		// Domyślny kąt to 0*
		resources.push_back(readShape(shape, shape, 0)->getResourceID());
	}
	
	// Wczytywanie parametrów graficznych platform..
	fscanf(map, "%hu\n", &size);
	for (usint i = 0; i < size; ++i) {
		fscanf(map,
				"%hu %d %d %d %d %hu %hu %f %f %f %f %hu %f %f %f %f %hu %hu %hu %hu %hu %hu %s\n",
				&script_id, &border[0], &border[1], &border[2], &border[3],
				&type, &repeat_movement, &max_distance.x, &max_distance.y,
				&velocity.x, &velocity.y, &state, &rect.x, &rect.y, &rect.w,
				&rect.h, &col.r, &col.g, &col.b, &col.a, &layer, &with_shape,
				shape);
		//
		Platform* platform = NULL;
		/**
		 * Wczytywanie kształtu
		 */
		if (with_shape) {
			platform =
					new IrregularPlatform(rect.x, rect.y, state,
							dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(
									shape)));
			dynamic_cast<IrregularPlatform*>(platform)->fitToWidth(rect.w);
		} else {
			platform = new Platform(rect.x, rect.y, rect.w, rect.h, col, state);
			platform->setBorder(border[0], border[1], border[2], border[3]);
		}
		//
		platform->layer = layer;
		platform->script_id = script_id;
		platform->setFillType(type);
		//
		if (velocity.x != 0 || velocity.y != 0) {
			platform->setMovingDir(velocity, max_distance, repeat_movement);
		}
		platform->compileList();
		//
		objects.push_back(platform);
	}
	// Obliczanie wymiarów!
	calcBounds();
	
	// Inicjacja fizyki!
	if (physics) {
		delete physics;
	}
	physics = new pEngine(bounds, 0.4f);
	for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
		physics->insert(*iter);
	}
	ResourceFactory::getIstance(physics);
	
	Vector<float> pos;
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
		//
		fscanf(map, "%f %f %f %f %[^\n]s\n", &rect.x, &rect.y, &rect.w, &rect.h,
				shape);
		//
		ResourceFactory::getIstance(physics).createObject(
				ResourceFactory::SCRIPT_BOX, rect.x, rect.y, rect.w, rect.h,
				NULL, shape);
	}
	return true;
}

void MapINFO::unload() {
	/**
	 * Szybsze niż AllocKiller
	 */
	for (usint i = 0; i < objects.size(); ++i) {
		delete objects[i];
	}
	
	ResourceFactory::getIstance(NULL).unloadObjects();
	// Usuwanie kształtów! Woolne!
	for (usint i = 0; i < resources.size(); ++i) {
		main_resource_manager.deleteResource(i);
	}
	if (physics) {
		delete physics;
	}
	//
	logEvent(Logger::LOG_INFO, "Pomyślnie usunięto mapę!");
}
