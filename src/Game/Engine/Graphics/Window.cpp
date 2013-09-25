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

/** Rozdzialka ekranu */
Vector<float> Engine::screen_bounds;

/** Czy okno jest otwarte? */
bool Engine::window_opened = true;
bool Engine::resolution_changed = false;
bool Engine::with_shaders = true;

/** Flagi glownej petli gry */

#define FPS 16
//#define BENCHMARK
#define FULLSCREEN

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

Vector<float> native_resolution;

/** Konstruktor */
Window::Window(const string& _title) :
				screen(NULL) {
	/** Inicjalizacja SDL */
	SDL_Init(SDL_INIT_EVERYTHING);
	
	/** Wyliczanie rozdzielczosci ekranu */
	screen_bounds.x = 640;
	screen_bounds.y = 480;
	
	/** Natywna rozdzielczosc */
	native_resolution = Window::getNativeResolution();
	
	/** Tworzenie okna */
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

/** Inicjacja okna */
void Window::init() {
	if (setupOpenGL()) {
		logEvent(Logger::LOG_INFO, "Okno skonfigurowane sukcesem!");
	} else {
		return;
	}
	
	/** Wczytywanie rdzennych elementow gry */
	openConfig();
	
	//
	SDL_Event event;
	Event key(Event::KEY_PRESSED);
	
	/** Shadery */
#ifdef BENCHMARK
	int frame_start = SDL_GetTicks();
	int frames = 0;
	//
	glText frame_count(oglWrapper::WHITE, "");
#endif
	while (window_opened) {
		/** Zmiana rozdzielczosci */
		if (resolution_changed) {
			SDL_FreeSurface(screen);
			if (screen_bounds.x == 0 || screen_bounds.y == 0) {
				screen_bounds = native_resolution;
			}
			screen = SDL_SetVideoMode(
					screen_bounds.x,
					screen_bounds.y,
					32,
					SDL_OPENGL | SDL_GL_DOUBLEBUFFER
#ifdef FULLSCREEN
							| SDL_FULLSCREEN
#endif
					);
			setupOpenGL();
			if (!menu) {
				loadScreens();
				if (with_shaders) {
					loadShadersPack();
				}
			}
			resolution_changed = false;
		}
#ifndef BENCHMARK
		int frame_start = SDL_GetTicks();
#endif
		/**
		 *
		 */
		SDL_GetMouseState(&mouse.pos.x, &mouse.pos.y);
		while (SDL_PollEvent(&event)) {
			if (active_screen) {
				switch (event.type) {
					case SDL_MOUSEBUTTONUP:
						active_screen->catchEvent(
								Event(Event::MOUSE_RELEASED, ' '));
						break;
						
					case SDL_MOUSEBUTTONDOWN:
						active_screen->catchEvent(
								Event(Event::MOUSE_PRESSED, ' '));
						break;
						
					case SDL_QUIT:
						window_opened = false;
						break;
				}
			}
		}
		Uint8 *keystate = SDL_GetKeyState(NULL);
		if (keystate[SDLK_ESCAPE]) {
			window_opened = false;
		}
		if (active_screen) {
			translateKeyEvent(keystate, SDLK_w, 'w', key, game);
			translateKeyEvent(keystate, SDLK_a, 'a', key, game);
			translateKeyEvent(keystate, SDLK_d, 'd', key, game);
			translateKeyEvent(keystate, SDLK_SPACE, '*', key, game);
		}
		
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();
		if (active_screen) {
			active_screen->drawObject(this);
		}
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
	if (with_shaders) {
		unloadShadersPack();
	}
}

/** Pobieranie rozdzielczosci ekranu */
Vector<float> Window::getNativeResolution() {
	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	//
	return Vector<float>(info->current_w, info->current_h);
}

/** Instalacja OpenGL */
bool Window::setupOpenGL() {
	glewExperimental = GL_TRUE;
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
		logEvent(Logger::LOG_ERROR, "Brak obslugi shaderow!");
		//
		with_shaders = false;
	}
	
	return true;
}

/** Usuwanie i sprzatanie po sobie */
Window::~Window() {
	SDL_FreeSurface(screen);
	SDL_Quit();
}
