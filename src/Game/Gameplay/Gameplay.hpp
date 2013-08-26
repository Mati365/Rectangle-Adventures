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
		public:
			Rect<float> pos;
			Body* focus;

			Camera(Body* _focus) :
							focus(_focus) {
				pos.w = WINDOW_WIDTH;
				pos.h = WINDOW_HEIGHT;
			}
			
			void updateCam(Window* _window) {
				if (!focus) {
					return;
				}
				pos.x = focus->x - pos.w / 2 + focus->w / 2;
				pos.y = focus->y - pos.h / 2 + focus->h / 2;
			}
	};
	
	/**
	 * Renderer paralaxy
	 */
	class ParalaxRenderer: public Renderer, public IntroBackground {
		protected:
			/** Mapa do głównego renderowania */
			MapINFO* map;

			/**
			 * Statycznych obiektów jest mniej
			 * AllocKiller nie powinien zmniejszyć
			 * wydajności
			 */
			deque<AllocKiller<Body> > static_objects;

			/** Kamera */
			Camera cam;
			float ratio;

			/** Rysowanie fioletowej szachownicy */
			bool draw_quad;
			bool rotate;

			/** Timer potrząsania */
			_Timer shake_timer;

		public:
			ParalaxRenderer(Body*, float, bool, MapINFO* = NULL);

			virtual void drawObject(Window*);

			/**
			 * Element statyczny np. element interface'u
			 */
			void addStaticObject(Body*);

			/** Potrząsanie ekranem */
			void shake();

			/** Odblokowywanie rotacji kamery */
			void enableRotate(bool _rotate) {
				rotate = _rotate;
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

			Camera* getCamera() {
				return &cam;
			}

			~ParalaxRenderer() {
				if (map) {
					safe_delete<MapINFO>(map);
				}
			}
	};
	
	/**
	 * Główny renderer mapy!
	 */
	class MapRenderer: public ParalaxRenderer, public EventListener {
#define DEFAULT_SHADOW_RADIUS 250

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

			ParalaxRenderer* addToParalax(MapINFO*, float, Body*);

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
				col_saturation[0] = col_saturation[1] = col_saturation[2] = 1.f;
			}
	};
}

#endif /* GAMEPLAY_HPP_ */
