/*
 * Game.cpp
 *
 *  Created on: 23-06-2013
 *      Author: mateusz
 */
#include "Screens.hpp"

#include "../../Gameplay/Gameplay.hpp"
#include "../LevelManager.hpp"

#include "../../Resources/Data/ResourceManager.hpp"

using namespace GameScreen;

Game::Game(const char* map_path) {
	lvl = new MapRenderer(
			new Character("Ufolud", 0, 0, NULL, Body::HERO),
			!map_path ?
					LevelManager::getInstance().getFirstMap() :
					loadMap(map_path));
}

void Game::catchEvent(const Event& event) {
	if (lvl) {
		lvl->catchEvent(event);
	}
}

void Game::drawObject(Window* window) {
	if (lvl) {
		lvl->drawObject(window);
	}
}
