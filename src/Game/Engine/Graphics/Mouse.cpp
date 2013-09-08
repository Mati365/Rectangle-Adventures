/*
 * Mouse.cpp
 *
 *  Created on: 08-09-2013
 *      Author: mateusz
 */
#include "Engine.hpp"

using namespace Engine;

/** Chowanie myszy */
void Engine::hideMouse() {
	SDL_ShowCursor(SDL_DISABLE);
}

/** Pokazanie myszy */
void Engine::showMouse() {
	SDL_ShowCursor(SDL_ENABLE);
}
