/*
 * LevelManager.cpp
 *
 *  Created on: 17-08-2013
 *      Author: mateusz
 */
#include "LevelManager.hpp"

#include "../Resources/Data/SoundManager.hpp"

string levels[MAP_COUNT] {
					"mapa5.txt",
					"mapa3.txt",
					"mapa7.txt",
					"mapa6.txt",
					"mapa4.txt",
					"mapa2.txt",
					"mapa.txt" };

/** Konstruktor */
LevelManager::LevelManager() :
				actual_map(0) {
}

/**
 * Wczytywanie nastepnej planszy
 * + Kasowanie ostatniej mapy ze ekranu game
 */
MapINFO* LevelManager::loadNextMap() {
	if (!game) {
		logEvent(
				Logger::LOG_ERROR,
				"Nie mogę wczytać nastepnej mapy! Koniec gry.");
		return nullptr;
	}
	
	/** Wczytywanie nowej mapy */
	if (actual_map + 1 < MAP_COUNT) {
		actual_map++;

		/** Savey */
		SaveManager& save = SaveManager::getInstance();
		Character* hero = game->getMapRenderer()->getHero();

		save.getSave()->stats[Save::POINTS] += hero->getStatus()->score;
		save.getSave()->stats[Save::LAST_LEVEL_INDEX] = actual_map;

		exportSave();
	} else {
		/** Koniec gry */
		openEnding();
	}

	/** Reload mapy */
	MapINFO* buffer = reloadMap();
	
	/** Obiekt dynamicznie alokowany */
	return buffer;
}

/** Reload calej mapy */
MapINFO* LevelManager::reloadMap() {
	if (actual_map > MAP_COUNT) {
		return nullptr;
	}
	
	MapINFO* buffer = loadMap(levels[actual_map].c_str());
	MapRenderer* map_renderer = game->getMapRenderer();
	
	map_renderer->setBufferMap(buffer);
	
	/** Gracz popelnia harakiri xD */
	map_renderer->getHero()->die();
	
	return buffer;
}

/** Wczytywanie calej planszy */
MapINFO* LevelManager::getFirstMap() {
	actual_map = 0;
	//
	return loadMap(levels[0].c_str());
}
