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
 * Menedzer poziomow, odpowiada za
 * usuwanie/wczytywanie listy map.
 * + Wzorzec singleton!
 * + Korzysta ze screenow!
 * + Korzysta z systemu plikow
 */
extern string levels[];

class LevelManager {
#define MAP_COUNT 3
		
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
		 * Zwraca NULL'a jesli koniec,
		 * jesli koniec to napisy końcowe
		 * i podsumowanie gry
		 */
		MapINFO* loadNextMap();

		/** Wczytywanie calej mapy od nowa */
		MapINFO* reloadMap();

		/**
		 * Pobieranie pierwszej mapy
		 * wczytanej z save
		 */
		MapINFO* getFirstMap();
};

#endif /* LEVELMANAGER_HPP_ */
