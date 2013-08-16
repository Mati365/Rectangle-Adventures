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
 */
class LevelManager {
	private:
		map<usint, const char*> maps;

		LevelManager() {
		}

	public:
};

#endif /* LEVELMANAGER_HPP_ */
