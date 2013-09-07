/*
 * LevelManager.cpp
 *
 *  Created on: 17-08-2013
 *      Author: mateusz
 */
#include "LevelManager.hpp"

string levels[2] { "mapa.txt", "mapa2.txt" };

/**
 * Konstruktor
 */
LevelManager::LevelManager() :
				actual_map(0) {
}

/**
 * Wczytywanie następnej planszy
 * + Kasowanie ostatniej mapy ze ekranu game
 */
MapINFO* LevelManager::loadNextMap() {
	if (!game) {
		logEvent(
				Logger::LOG_ERROR,
				"Nie mogę wczytać nastepnej mapy! Koniec gry.");
		return NULL;
	}
	
	// Wczytywanie nowej mapy
	if (actual_map + 1 < MAP_COUNT) {
		actual_map++;
	}
	MapINFO* buffer = reloadMap();
	
	// Obiekt dynamicznie alokowany
	return buffer;
}

/**
 * Wczytywanie całej mapy od nowa
 */
MapINFO* LevelManager::reloadMap() {
	if (actual_map > MAP_COUNT) {
		return NULL;
	}
	MapINFO* buffer = loadMap(levels[actual_map].c_str());
	MapRenderer* map_renderer = game->getMapRenderer();
	
	map_renderer->setBufferMap(buffer);
	
	// Gracz musi się zabić ;)
	map_renderer->getHero()->die();
	
	return buffer;
}

/**
 * Wczytywanie pierwszej mapy
 */
MapINFO* LevelManager::getFirstMap() {
	actual_map = 1;
	//
	return loadMap(levels[0].c_str());
}
