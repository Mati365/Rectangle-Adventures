/*
 * Engine.hpp
 *
 *  Created on: 02-03-2013
 *      Author: mati
 */

#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_
#include <deque>
#include <map>

#include "../Graphics/Engine.hpp"
#include "../../Tools/Tools.hpp"

#define STATIC_LAYER 0
#define MAX_LAYER 2

#define MAX_QUADTREE_LEVEL 3

namespace Physics {
	using namespace std;
	using namespace Engine;
	using namespace Memory;
	
	template<typename T> class Rect {
		public:
			T x;
			T y;
			T w;
			T h;

			Rect(T _x = 0, T _y = 0, T _w = 0, T _h = 0) :
							x(_x),
							y(_y),
							w(_w),
							h(_h) {
			}
			
			operator Vector<T>() const {
				return Vector<T>(x, y);
			}
			
			inline void getFromVec(const Vector<float>& _vec) {
				//w = h = 0;
				x = _vec.x;
				y = _vec.y;
			}
			
			inline Rect<T>& operator+=(const Vector<T>& _vec) {
				x += _vec.x;
				y += _vec.y;
				//
				return *this;
			}
			/**
			 * Czy zawiera obiekt
			 */
			inline bool contains(const Rect<T>& _child) {
				return (_child.x >= x && _child.x + _child.w <= x + w
						&& _child.y >= y && _child.y + _child.h <= y + h);
			}
			
			/** Czy koliduje z obiektem */
			inline bool intersect(const Rect<T>& _body) {
				return (_body.x + _body.w >= x && _body.x <= x + w
						&& _body.y + _body.h >= y && _body.y <= y + h);
			}
	};
	
	/** Odwrócenie kierunku */
	usint invertDir(usint);

	/** Vertykalny czy Hpryzontalny */
	bool isHorizontalDir(usint);
	
	/** Odepchnięcie ciała */
	class Body;
	void dodgeBody(Body*, usint, float);

	/**
	 * Todo:
	 * + Przebudowa, sprawdzanie kolizji tylko
	 * na widocznym skrawku ekranu!
	 * + Usuwanie poruszających się i dodawanie
	 * ich spowrotem!
	 * + Dodawanie tylko do jednego quad'u!
	 */
	class Body;
	class QuadTree: public Renderer {
		private:
			Rect<float> rect;
			deque<Body*> bodies;

			usint level;
			/**
			 * STAŁE
			 */
			QuadTree* parent;
			QuadTree* NW;
			QuadTree* NE;
			QuadTree* SW;
			QuadTree* SE;

		public:
			QuadTree(QuadTree*, const Rect<float>&, usint);

			/** Aktualizacja drzewa! */
			void subdive();

			void insertGroup(deque<Body*>*);
			void insert(Body*);

			/** Kasowanie */
			bool remove(Body*);
			bool remove(usint);

			void getBodiesAt(Rect<float>&, deque<Body*>&);
			void update(Rect<float>&);

			virtual void drawObject(Window*);

			~QuadTree();

		private:
			bool insertToSubQuad(Body*, bool);
	};
	
	/** Główny silnik fizyczny */
	class pEngine {
		public:
			/** Zwrot */
			enum Dir {
				NONE,
				RIGHT,
				LEFT,
				UP,
				DOWN
			};

			/** Konfiguracja */
			enum Flags {
				GRAVITY_DISABLED = 1 << 1,
				PAUSE = 1 << 2
			};

		private:
			QuadTree* quadtree;
			Rect<float> bounds;

			/**
			 * Fizyka sprawdzana jest tylko i
			 * wyłącznie na widocznym ekranie!
			 */
			Rect<float> active_range;
			deque<Body*> visible_bodies;

			float gravity_speed;

			/**
			 *  Wszystkie obiekty dodane do silnika
			 *  wykorzystywane w skryptach
			 */
			deque<Body*> list;

			/** Flagi dla silnika */
			usint config;

			/** Uśpienie */
			_Timer sleep_timer;

		public:
			pEngine(const Rect<float>&, float);

			/** Obszar widoczny! */
			deque<Body*>* getVisibleBodies() {
				return &visible_bodies;
			}
			
			void setActiveRange(const Rect<float>& _active_range) {
				active_range = _active_range;
			}
			
			/** Dodawanie obiektu */
			void insert(Body*, bool = false);

			/** Usuwanie obiektu */
			bool remove(Body*);

			void clear() {
				list.clear();
			}
			
			/** Test kolizji */
			bool collide(const Body*, const Body*) const;

			/** Poruszenie się i test kolizji */
			bool moveAndCheck(float, float, Body*, const Body*);

			/** Odświeżenie świata */
			void updateWorld();

			/** Ustawienie lagu */
			void setSleep(usint);

			/** Dodawanie konfiguracji do silnika */
			void addConfig(usint _config) {
				ADD_FLAG(config, _config);
			}
			
			void removeConfig(usint _config) {
				UNFLAG(config, _config);
			}
			
			usint getConfig() const {
				return config;
			}
			
			QuadTree* getQuadTree() const {
				return quadtree;
			}
			
			Rect<float>& getBounds() {
				return bounds;
			}
			
			deque<Body*>* getList() {
				return &list;
			}
			
			float getGravitySpeed() const {
				return gravity_speed;
			}
			
			~pEngine();

		private:
			/** Aktualizacja poruszania się obiektu */
			void updateBodyMovement(Body*);

			/** Czy warto sprawdzać dla niego kolizje */
			bool isBodyActive(Body*);

			/** Czy się porusza? */
			bool isMoving(Body*);

			/** Sprawdzenie kolizji między obiektami z listy */
			void checkCollisions(deque<Body*>&);

			/** Detekcja kolizji */
			usint checkVerticalCollision(Body*, Body*);
			usint checkHorizontalCollision(Body*, Body*);
	};
	
	/**
	 * Obiekt podlegający fizyce
	 */
	class Body: public Rect<float>, public Renderer {
#define DEFAULT_ROUGHNESS 0.85f
			
		public:
			Body* collisions[4];

			/**
			 * Flaga obiektu, czym on jest?
			 */
			enum Type {
				HERO,
				ENEMY,
				PLATFORM,
				SCORE, // punkty
				BULLET, // pocisk
				TRIGGER, // event skrtpytu
				SPIKES, // kolce
				LADDER, // drabina
				LIANE, // liana
				PORTAL, // portal
				KILLZONE // strefa śmierci gracza
			};

			/**
			 * State, stan obiektu: czy podlega fizyce?
			 */
			enum State {
				NONE = 1 << 0,
				STATIC = 1 << 1,
				HIDDEN = 1 << 2,
				BACKGROUND = 1 << 3, // tło nie oddziaływujące
				FLYING = 1 << 4, // latający np. poruszające się platformy
				/**
				 * Element pomiędzy widocznym ekranem
				 * a widocznym musi nie podlegać fizyce
				 */
				BUFFERED = 1 << 5
			};

			/** Stan fizyczny */
			usint state;

			Vector<float> velocity;

			/** Chropowatość */
			float roughness;
			float weight;

			/** Flaga, typ obiektu  */
			usint type;

			/** Typ fabrykowy */
			usint factory_type;

			usint layer;
			usint script_id;

			/** Orientacja */
			usint orientation;

			/** Długość życia ciała */
			_Timer life_timer;

			/** Silnik fizyczny */
			pEngine* physics;

			Body();

			Body(float, float, float, float, float = DEFAULT_ROUGHNESS, float =
					1.f, usint = NONE);

			/** Nie wszystko musi mieć callback */
			virtual void catchCollision(pEngine*, usint, Body*) {
			}
			
			/** Długość życia do skasowania! */
			void setMaxLifetime(usint);

			/** Wymiary! */
			void setBounds(float, float, float, float);

			/** Nadawanie type! */
			void setType(usint _type) {
				type = _type;
			}
			
			/** Nadawanie stanu! */
			void setState(usint _state) {
				state = _state;
			}

			virtual ~Body() {
			}
	};
}

#endif /* PHYSICS_HPP_ */
