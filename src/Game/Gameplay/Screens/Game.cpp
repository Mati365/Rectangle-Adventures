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
	MapINFO* paralax_1 = loadMap("paralax_1.txt");

	lvl = new MapRenderer(NULL, map);
	//
	hero = new Character(
			"Ufolud",
			map->hero_bounds.x,
			map->hero_bounds.y,
			dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(
					"player")),
			Body::HERO);
	hero->fitToWidth(map->hero_bounds.w);

	lvl->addToParalax(paralax_1, 0.45f, hero);
	lvl->addWeather(MapRenderer::SNOWING);

	lvl->getPhysics()->insert(hero);
	lvl->setHero(hero);
	//
	/**
	 * TEST
	 */
	char d[] =
			"SHOW_SPLASH Tymczasem%gdzies%w%odleglej%galaktyce.. SHOW_MESSAGE Sterowanie Poruszanie%sie:%%%w%-%skok%%%%a%-%lewo%%%%d%-%prawo CREATE_OBJECT 5 550.0 130.0 CREATE_OBJECT 5 650.0 130.0 ";
	Script* script = Interpreter::getIstance().compile(d);
	Trigger* trigger = new Trigger(script, 200, 350, 300, 300);
	lvl->getPhysics()->insert(trigger);
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
