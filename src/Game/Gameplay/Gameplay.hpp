/*
 * Gameplay.hpp
 *
 *  Created on: 23-03-2013
 *      Author: mati
 */

#ifndef GAMEPLAY_HPP_
#define GAMEPLAY_HPP_
#include <deque>

#include "Event.hpp"

#include "../Engine/Graphics/Engine.hpp"
#include "../Engine/Graphics/Fonts.hpp"
#include "./Objects/Objects.hpp"

#include "../Tools/Tools.hpp"

#include "../Resources/Data/ResourceManager.hpp"
#include "../Resources/Data/Resources.hpp"

#include "../GUI/GUI.hpp"
#include "./HUD/HUD.hpp"

using namespace std;

using namespace oglWrapper;

namespace Gameplay {
	using namespace Memory;
	using namespace Physics;
	using namespace GUI;
	
	class Camera {
			/**
			 * Focus będzie bardziej na
			 * dole ekranu
			 */
#define Y_SPACE 100
			
		private:
			Rect<float> pos;
			Body* focus;

			/** Czy scrollowanie do focusa? */
			bool scrolling;

		public:
			/**
			 * Pozycja kamery to nie pozycja względem krawędzi!!
			 */
			Camera(Body*);

			/** Odświeżanie pocycji kamery */
			void updateCam(Window*);

			/** Ustawienie focusa */
			void setFocus(Body*);

			/** Skrollowanie do.. */
			void scrollTo(Body*);

			/** Pobieranie pozycji względem krawędzi okna */
			Vector<float> getFocusScreenPos() const;

			Body* getFocus() {
				return focus;
			}
			
			Rect<float>* getPos() {
				return &pos;
			}
			
			/** nullptr zwraca ostatni focus */
			static Camera& getFor(Body* focus = nullptr) {
				static Camera camera(focus);
				if (focus) {
					camera.focus = focus;
				}
				//
				return camera;
			}
	};
	
	/**
	 * Renderer paralaxy
	 */
	class ParalaxRenderer: public Renderer, public IntroBackground {
		public:
			/** Konfiguracja renderera */
			enum RendererConfig {
				DRAW_QUAD = 1 << 1, // Rysowanie siatki z quadtree
				ROTATION = 1 << 2, // Rotacja mapy
				PARALLAX = 1 << 3 // Brak rysowania gracza
			};

		protected:
			
			/** Konfiguracja */
			usint config;

			/** Mapa do głównego renderowania */
			MapINFO* map;

			/**
			 * Statycznych obiektów jest mniej
			 * AllocKiller nie powinien zmniejszyć
			 * wydajności
			 */
			deque<AllocKiller<Body> > static_objects;

			/** Kamera */
			float ratio;

			/** Timer potrząsania */
			_Timer shake_timer;

		public:
			ParalaxRenderer(Body*, float, MapINFO* = nullptr);

			virtual void drawObject(Window*);

			/** Element statyczny np. element interface'u  */
			void addStaticObject(Body*);

			/** Potrząsanie ekranem */
			void shake();

			/** Ustawienie konfiguracji */
			void setConfig(usint _config) {
				config = _config;
			}
			
			float getRatio() const {
				return ratio;
			}
			
			usint getConfig() const {
				return config;
			}
			
			virtual Character* getHero() {
				return NULL;
			}
			
			MapINFO* getMap() {
				return map;
			}
			
			pEngine* getPhysics() {
				return map->physics;
			}
			
			~ParalaxRenderer() {
				safe_delete<MapINFO>(map);
			}
	};
	
	/**
	 * Główny renderer mapy!
	 */
	class MapRenderer: public ParalaxRenderer, public EventListener {
#define DEFAULT_SHADOW_RADIUS 230
#define DEFAULT_CAM_RATIO .95f
			
		public:
			enum Weather {
				NONE,
				SNOWING,
				SHAKE,
				FIREWORKS
			};

		private:
			/** HUD */
			MessageRenderer msg;

			/** Rzutowanie z paralaxy */
			Character* hero;

			/** Paralaxy za główną mapą */
			deque<ParalaxRenderer*> paralax_background;

			/** Hud w menu jest zablokowany */
			bool hud_enabled;

			/** Główny shader renderu */
			usint main_shader_id;

			/** Promień cienia wokół kamery */
			float shadow_radius;

			/** Nasycenie kolorów w shaderze */
			float col_saturation[3];

			/** mapa zastępcza podczas ładowania */
			MapINFO* buffer_map;

			/** Zamiana mapy po narysowaniu klatki */
			bool buffer_swap_required;

		public:
			MapRenderer(Body*, MapINFO*);

			virtual void catchEvent(const Event&);
			virtual void drawObject(Window*);

			/** Dodawanie paralaksy */
			ParalaxRenderer* addToParalax(MapINFO*, float, Body*, usint = 0);

			/** Włączanie HUDu */
			void enableHUD(bool _hud_enabled) {
				hud_enabled = _hud_enabled;
			}
			
			/** Ustawienie głównego shaderu */
			void setMainShader(usint _main_shader_id) {
				main_shader_id = _main_shader_id;
			}
			
			/**
			 * Ustawienie mapy buforowanej do
			 * wczytywania nowych poziomów
			 */
			void swapBufferMap();

			void setBufferMap(MapINFO* buffer) {
				buffer_map = buffer;
			}
			
			/** Mapa buforowana */
			MapINFO* getBufferMap() {
				return buffer_map;
			}
			
			/** Wczytywanie mapy */
			void setMap(MapINFO*);

			/** Resetowanie głównego gracza */
			void resetHero();

			/** Pogoda */
			void addWeather(usint);
			void setHero(Character*);

			/** Game over */
			void showGameOver();

			/** Metoda przesłonięta! */
			virtual Character* getHero() {
				return hero;
			}
			
			MessageRenderer* getMessageRenderer() {
				return &msg;
			}
			
			~MapRenderer();

		private:
			/** Resetowanie saturacji barw */
			inline void resetColorSaturation() {
				col_saturation[0] = 1.1f; // bardziej karmelkowy ekran
				col_saturation[1] = col_saturation[2] = 1.f;
			}

			/** Obliczanie współczynnika przesunięcia */
			void calcCameraRatio();
	};
}

#endif /* GAMEPLAY_HPP_ */
