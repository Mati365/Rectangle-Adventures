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
class LevelManager {
	private:
		int actual_map;
		deque<const char*> maps;

		LevelManager() :
						actual_map(-1) {
		}

	public:
		/**
		 * Zwraca NULL'a jeśli koniec,
		 * jeśli koniec to napisy końcowe
		 * i podsumowanie gry
		 */
		MapINFO* loadNextMap();
};

#endif /* LEVELMANAGER_HPP_ */
