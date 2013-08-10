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
	
	class ParalaxRenderer: public Renderer, public IntroBackground {
		protected:
			MapINFO* map;
			/**
			 * Statycznych obiektów jest mniej
			 * AllocKiller nie powinien zmniejszyć
			 * wydajności
			 */
			deque<AllocKiller<Body> > static_objects;

			// Kamera
			Camera cam;
			float ratio;

			// Rysowanie fioletowej szachownicy
			bool draw_quad;
			bool rotate;

		public:
			ParalaxRenderer(Body*, float, bool, MapINFO*);

			virtual void drawObject(Window*);

			/**
			 * Element statyczny np. element interface'u
			 */
			void addStaticObject(Body*);

			virtual Character* getHero() {
				return NULL;
			}
			
			void enableRotate(bool _rotate) {
				rotate = _rotate;
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
	};
	
	/**
	 * Główny renderer mapy!
	 */
	class MapRenderer: public ParalaxRenderer, public EventListener {
#define SHADOW_RADIUS 250

		public:
			enum Weather {
				SNOWING
			};

		private:
			// HUD
			MessageRenderer msg;

			Character* hero;

			// Paralaxy za główną mapą
			deque<ParalaxRenderer*> paralax_background;

			// Hud w menu jest zablokowany
			bool hud_enabled;

			// Główny shader renderu
			usint main_shader_id;

			// Promień cienia wokół kamery
			float shadow_radius;

		public:
			MapRenderer(Body*, MapINFO*);

			virtual void catchEvent(const Event&);
			virtual void drawObject(Window*);

			ParalaxRenderer* addToParalax(MapINFO*, float, Body*);

			// Włączanie HUDu
			void enableHUD(bool _hud_enabled) {
				hud_enabled = _hud_enabled;
			}
			
			// Ustawienie głównego shaderu
			void setMainShader(usint _main_shader_id) {
				main_shader_id = _main_shader_id;
			}

			// Pogoda :-)
			void addWeather(usint);
			void setHero(Character*);

			// Metoda przesłonięta!
			virtual Character* getHero() {
				return hero;
			}
			
			MessageRenderer* getMessageRenderer() {
				return &msg;
			}
			
			~MapRenderer();
	};
}

#endif /* GAMEPLAY_HPP_ */
