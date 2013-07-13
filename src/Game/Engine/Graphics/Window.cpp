/*
 * Engine.cpp
 *
 *  Created on: 23-02-2013
 *      Author: mati
 */
#include <GL/glu.h>

#include "../../Tools/Logger.hpp"
#include "../../Gameplay/Screens/Screens.hpp"

using namespace Engine;
using namespace Gameplay;
using namespace GameScreen;
using namespace GUI;

#define FPS 17

bool Engine::window_opened = true;

//

void translateKeyEvent(Uint8* keystate, Uint8 key, char translated,
		Event& event, Screen* renderer) {
	if (keystate[key]) {
		event.key = translated;
	}
	if (event.key != ' ') {
		renderer->catchEvent(event);
	}
	event.key = ' ';
}

Window::Window(const Vector<usint>& _bounds, const string& _title) :
		screen(NULL),
		bounds(_bounds) {
	screen = SDL_SetVideoMode(bounds.x, bounds.y, 32,
	SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGL);
	if (!screen) {
		return;
	}
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_WM_SetCaption(_title.c_str(), _title.c_str());
}

void Window::init() {
	loadScreens();
	//
	if (!menu) {
		logEvent(Logger::LOG_INFO,
				"Nastąpił problem podczas wczytywania menu.");
		return;
	}
	active_screen = dynamic_cast<Game*>(menu);
	//
	if (setupOpenGL()) {
		logEvent(Logger::LOG_INFO, "Okno skonfigurowane sukcesem!");
	} else {
		return;
	}
	//
	SDL_Event event;
	Event key(Event::KEY_PRESSED);

	while (window_opened) {
		int frame_start = SDL_GetTicks();
		/**
		 *
		 */
		SDL_GetMouseState(&mouse.pos.x, &mouse.pos.y);
		Camera* cam = active_screen->getMapRenderer()->getCamera();
		//
		mouse.pos.x += cam->pos.x;
		mouse.pos.y += cam->pos.y;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_MOUSEBUTTONUP:
					active_screen->catchEvent(
							Event(Event::MOUSE_RELEASED, ' '));
					break;

				case SDL_MOUSEBUTTONDOWN:
					active_screen->catchEvent(Event(Event::MOUSE_PRESSED, ' '));
					break;

				case SDL_QUIT:
					window_opened = false;
					break;

				default:
					break;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		active_screen->drawObject(this);
		SDL_GL_SwapBuffers();
		/**
		 *
		 */
		int frame_time = SDL_GetTicks() - frame_start;
		if (frame_time <= FPS) {
			frame_start = SDL_GetTicks();
			//
			SDL_Delay(FPS - frame_time);
		}
		/**
		 *
		 */
		Uint8 *keystate = SDL_GetKeyState(NULL);
		if (keystate[SDLK_ESCAPE]) {
			window_opened = false;
		}
		translateKeyEvent(keystate, SDLK_w, 'w', key, game);
		translateKeyEvent(keystate, SDLK_a, 'a', key, game);
		translateKeyEvent(keystate, SDLK_d, 'd', key, game);
		translateKeyEvent(keystate, SDLK_SPACE, '*', key, game);
	}
	//
	unloadScreens();
}

bool Window::setupOpenGL() {
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, bounds.x, bounds.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, bounds.x, bounds.y, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (!GL_ARB_vertex_shader || !GL_ARB_fragment_shader) {
		logEvent(Logger::LOG_ERROR, "Brak obsługi shaderów!");
		return false;
	}
	return true;
}

Window::~Window() {
	SDL_FreeSurface(screen);
	SDL_Quit();
}
