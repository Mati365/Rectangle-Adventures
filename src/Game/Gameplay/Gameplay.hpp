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
			}

			void updateCam(Window* _window) {
				if (!focus) {
					return;
				}
				if (pos.w == 0 || pos.h == 0) {
					pos.w = _window->getBounds()->x;
					pos.h = _window->getBounds()->y;
				}
				pos.x = focus->x - _window->getBounds()->x / 2 + focus->w / 2;
				pos.y = focus->y - _window->getBounds()->y / 2 + focus->h / 2;
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
			deque<AllocKiller<Renderer> > static_objects;

			Camera cam;
			float ratio;

			// Rysowanie fioletowej szachownicy
			bool draw_quad;

		public:
			ParalaxRenderer(Body*, float, bool, MapINFO*);

			void addStaticObject(Renderer*);

			virtual void drawObject(Window*);

			virtual Character* getHero() {
				return NULL;
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
		public:
			enum Weather {
				SNOWING
			};

		private:
			MessageRenderer msg;
			Character* hero;
			deque<ParalaxRenderer*> paralax_background;

			bool hud_enabled;

		public:
			MapRenderer(Body*, MapINFO*);

			ParalaxRenderer* addToParalax(MapINFO*, float, Body*);

			// Włączanie HUDu
			void enableHUD(bool _hud_enabled) {
				hud_enabled = _hud_enabled;
			}

			// Pogoda :-)
			void addWeather(usint);
			void setHero(Character*);

			virtual void catchEvent(const Event&);
			virtual void drawObject(Window*);

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
