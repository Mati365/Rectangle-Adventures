/*
 * Engine.cpp
 *
 *  Created on: 23-02-2013
 *      Author: mati
 */
#include <sys/time.h>
#include <GL/glew.h>

#include "../../Gameplay/Screens/Screens.hpp"

#include "../../Tools/Logger.hpp"

using namespace Engine;
using namespace Gameplay;
using namespace GameScreen;
using namespace GUI;

#define FPS 9
//#define BENCHMARK

bool Engine::window_opened = true;

//

void translateKeyEvent(Uint8* keystate, Uint16 key, char translated,
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
	screen = SDL_SetVideoMode(
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			32,
			SDL_OPENGL | SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER);
	if (!screen) {
		return;
	}
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_WM_SetCaption(_title.c_str(), _title.c_str());

	//
	if (screen->flags & SDL_OPENGL) {
		logEvent(Logger::LOG_INFO, "OpenGL obsługiwany!");
	}
}

void Window::init() {
	if (setupOpenGL()) {
		logEvent(Logger::LOG_INFO, "Okno skonfigurowane sukcesem!");
	} else {
		return;
	}
	/**
	 * Wczytywanie rdzennych elementów gry
	 */
	loadShadersPack();
	loadSoundsPack();
	loadScreens();
	//
	if (!menu) {
		logEvent(
				Logger::LOG_INFO,
				"Nastąpił problem podczas wczytywania menu.");
		return;
	}
	active_screen = game;
	splash->pushTitle(
			"..cziken58 prezentuje..",
			520,
			getShapeFromFilesystem("iluzja_trojkat.txt", 6.f));
	splash->pushTitle(
			"...gre nie wczytujaca ani jednego sprite...",
			690,
			getShapeFromFilesystem("pudlo.txt", -6.f));
	splash->pushTitle("Rect Adventures", 490);
	splash->endTo(menu);

	//
	SDL_Event event;
	Event key(Event::KEY_PRESSED);
	
	/**
	 * Shadery
	 */
#ifdef BENCHMARK
	int frame_start = SDL_GetTicks();
	int frames = 0;
	//
	glText frame_count(oglWrapper::WHITE, "");
#endif
	while (window_opened) {
#ifndef BENCHMARK
		int frame_start = SDL_GetTicks();
#endif
		//
		/**
		 *
		 */
		SDL_GetMouseState(&mouse.pos.x, &mouse.pos.y);
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
			}
		}
		Uint8 *keystate = SDL_GetKeyState(NULL);
		if (keystate[SDLK_ESCAPE]) {
			window_opened = false;
		}
		translateKeyEvent(keystate, SDLK_w, 'w', key, game);
		translateKeyEvent(keystate, SDLK_a, 'a', key, game);
		translateKeyEvent(keystate, SDLK_d, 'd', key, game);
		translateKeyEvent(keystate, SDLK_SPACE, '*', key, game);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		active_screen->drawObject(this);
#ifdef BENCHMARK
		frame_count.printText(WINDOW_WIDTH - 50, 20);
#endif
		glFlush();
		SDL_GL_SwapBuffers();
		
#ifndef BENCHMARK
		int frame_time = SDL_GetTicks() - frame_start;
		if (frame_time <= FPS) {
			frame_start = SDL_GetTicks() - FPS;
			//
			SDL_Delay(FPS - frame_time);
		}
#else
		frames++;
		//
		int frame_time = SDL_GetTicks() - frame_start;
		if (frame_time >= 1000) {
			frame_start = SDL_GetTicks();
			//
			frame_count.setString("FPS: " + Convert::toString<int>(frames), -1);
			frames = 0;
		}
#endif
	}
	wavPlayer::getInstance().closeMixAudio();
	//
	unloadSoundsPack();
	unloadScreens();
	unloadShadersPack();
}

bool Window::setupOpenGL() {
	glewInit();
	
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
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
