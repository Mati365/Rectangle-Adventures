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
	MapINFO* buffer = reloadMap();
	if (actual_map + 1 < MAP_COUNT) {
		actual_map++;
	}

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
	/*
	 MapINFO* _last_buffer = game->getMapRenderer()->getBufferMap();
	 if (_last_buffer) {
	 delete _last_buffer;
	 }
	 */
	MapINFO* buffer = loadMap(levels[actual_map].c_str());
	game->getMapRenderer()->setBufferMap(buffer);

	return buffer;
}

/**
 * Wczytywanie pierwszej mapy
 */
MapINFO* LevelManager::getFirstMap() {
	return loadMap(levels[0].c_str());
}
