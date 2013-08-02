/*
 * Game.cpp
 *
 *  Created on: 23-06-2013
 *      Author: mateusz
 */
#include "Screens.hpp"

#include "../../Gameplay/Gameplay.hpp"
#include "../../Resources/Data/ResourceManager.hpp"

using namespace GameScreen;

Game::Game(const char* map_path) {
	MapINFO* map = loadMap(map_path);
	
	lvl = new MapRenderer(NULL, map);
	//
	hero = new Character("Ufolud", map->hero_bounds.x, map->hero_bounds.y,
			getShapePointer("player"), Body::HERO);
	hero->fitToWidth(map->hero_bounds.w);
	//
	lvl->addWeather(MapRenderer::SNOWING);
	lvl->getPhysics()->insert(hero);
	lvl->setHero(hero);
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
