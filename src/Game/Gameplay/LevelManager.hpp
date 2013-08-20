/*
 * LevelManager.hpp
 *
 *  Created on: 16-08-2013
 *      Author: mateusz
 */

#ifndef LEVELMANAGER_HPP_
#define LEVELMANAGER_HPP_
#include "../Resources/Data/Resources.hpp"

#include "Screens/Screens.hpp"

using namespace GameScreen;

/**
 * Menedżer poziomów, odpowiada za
 * usuwanie/wczytywanie listy map.
 * + Wzorzec singleton!
 * + Korzysta ze screenów!
 * + Korzysta z systemu plików
 */
extern string levels[];

class LevelManager {
#define MAP_COUNT 2

	private:
		int actual_map;

		LevelManager();

	public:
		static LevelManager& getInstance() {
			static LevelManager manager;
			//
			return manager;
		}

		/**
		 * Zwraca NULL'a jeśli koniec,
		 * jeśli koniec to napisy końcowe
		 * i podsumowanie gry
		 */
		MapINFO* loadNextMap();

		/**
		 * Wczytywanie całej mapy od nowa
		 */
		MapINFO* reloadMap();

		/**
		 * Pobieranie pierwszej mapy
		 * wczytanej z save
		 */
		MapINFO* getFirstMap();
};

#endif /* LEVELMANAGER_HPP_ */
