/*
 * Engine.cpp
 *
 *  Created on: 23-02-2013
 *      Author: mati
 */
#include <sys/time.h>

#include "../../Gameplay/Screens/Screens.hpp"

#include "../../Tools/Logger.hpp"

using namespace Engine;
using namespace Gameplay;
using namespace GameScreen;
using namespace GUI;

/** Natywna rozdzielczośc ekranu! */
Vector<float> Engine::screen_bounds;

/** Czy okno jest otwarte? */
bool Engine::window_opened = true;

/** Flagi pętli gry */

#define FPS 16
#define BENCHMARK
//#define FULLSCREEN
#define VGA_RESOLUTION

/** Konwersja Uint8 do char */
void translateKeyEvent(Uint8* keystate, Uint16 key, char translated,
		Event& event, Screen* renderer) {
	if (keystate[key]) {
		event.key = translated;
	}
	if (event.key != ' ' && renderer) {
		renderer->catchEvent(event);
	}
	event.key = ' ';
}

/**
 * Konstruktor
 */
Window::Window(const string& _title) :
				screen(NULL) {
	// Inicjalizacja SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	
	// Wyliczanie rozdzielczości ekranu!
	screen_bounds = getNativeResolution();
	
	// Tworzenie okna
	screen = SDL_SetVideoMode(
			screen_bounds.x,
			screen_bounds.y,
			32,
			SDL_OPENGL | SDL_GL_DOUBLEBUFFER
#ifdef FULLSCREEN
					| SDL_FULLSCREEN
#endif
					);
	if (!screen) {
		return;
	}
	SDL_WM_SetCaption(_title.c_str(), _title.c_str());
	
	//
	if (IS_SET(screen->flags, SDL_OPENGL)) {
		logEvent(Logger::LOG_INFO, "OpenGL obsługiwany!");
	}
}

/**
 * Inicjacja okna
 */
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
	loadScreens();
	//
	if (!menu) {
		logEvent(
				Logger::LOG_INFO,
				"Nastąpił problem podczas wczytywania menu.");
		return;
	}
	active_screen = menu;
	/**
	 splash->endTo(menu);
	 splash->pushTitle("Mati365 presents..", 400, nullptr);
	 splash->pushTitle(
	 "Rect Adventures",
	 400,
	 readShape("iluzja_trojkat.txt", "iluzja_trojkat.txt", 33.f));
	 */

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
		
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
		active_screen->drawObject(this);
#ifdef BENCHMARK
		frame_count.printText(screen_bounds.x - 50, 70);
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
			frame_start = SDL_GetTicks() + FPS;
			//
			frame_count.setString("FPS: " + Convert::toString<int>(frames), -1);
			frames = 0;
		}
#endif
	}
	//
	unloadScreens();
	unloadShadersPack();
}

/**
 * Pobieranie natywnej rozdzielczości
 */
Vector<float> Window::getNativeResolution() {
#ifndef VGA_RESOLUTION
	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	//
	return Vector<float>(info->current_w, info->current_h);
#else
	return Vector<float>(640, 480);
#endif
}

/**
 * Instalacja OpenGL
 */
bool Window::setupOpenGL() {
	glewInit();
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	
	glDepthMask(GL_FALSE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.5f);
	
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glViewport(0, 0, screen_bounds.x, screen_bounds.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screen_bounds.x, screen_bounds.y, 0, -1, 1);
	
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
