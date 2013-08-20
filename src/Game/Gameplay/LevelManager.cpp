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
LevelManager::LevelManager(string* levels, usint count) :
				actual_map(-1) {
	for (usint i = 0; i < count; ++i) {
		maps.push_back(levels[i].c_str());
	}
}

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

	// Wczytywanie nowej mapy
	MapINFO* buffer = reloadMap();
	maps.pop_front();

	// Obiekt dynamicznie alokowany
	return buffer;
}

/**
 * Wczytywanie całej mapy od nowa
 */
MapINFO* LevelManager::reloadMap() {
	const char* path = maps.front();

	if (strlen(path) == 0) {
		return NULL;
	}
	/*
	 MapINFO* _last_buffer = game->getMapRenderer()->getBufferMap();
	 if (_last_buffer) {
	 delete _last_buffer;
	 }
	 */
	MapINFO* buffer = loadMap(path);
	game->getMapRenderer()->setBufferMap(buffer);

	return buffer;
}
