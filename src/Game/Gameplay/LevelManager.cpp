/*
 * LevelManager.cpp
 *
 *  Created on: 17-08-2013
 *      Author: mateusz
 */
#include "LevelManager.hpp"

/**
 * Wczytywanie następnej planszy
 * + Kasowanie ostatniej mapy ze ekranu game
 */
MapINFO* LevelManager::loadNextMap() {
	if (maps.empty() || !game) {
		logEvent(
				Logger::LOG_ERROR,
				"Nie mogę wczytać nastepnej mapy! Koniec gry.");
		return NULL;
	}

	// Czyszczenie ostatniej mapy
	MapINFO* last_map = game->getMapRenderer()->getMap();
	if (last_map) {
		delete last_map;
	}

	// Wczytywanie nowej
	const char* path = maps.front();
	maps.pop_front();

	if (strlen(path) == 0) {
		return NULL;
	}

	MapINFO* buffer = loadMap(path);
	game->getMapRenderer()->setBufferMap(buffer);

	// Obiekt dynamicznie alokowany
	return buffer;
}

