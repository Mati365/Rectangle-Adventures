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

#define IS_SET(source, flag) (source&flag)

#define UNFLAG(source, flag) (source &= ~flag)
#define ADD_FLAG(source, flag) (source |= flag)

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
			/**
			 * Czy zawiera obiekt
			 */
			inline bool contains(const Rect<T>& _child) {
				return (_child.x >= x && _child.x + _child.w <= x + w
						&& _child.y >= y && _child.y + _child.h <= y + h);
			}
			
			/**
			 * Czy koliduje z obiektem
			 */
			inline bool intersect(const Rect<T>& _body) {
				return (_body.x + _body.w >= x && _body.x <= x + w
						&& _body.y + _body.h >= y && _body.y <= y + h);
			}
	};
	
	/**
	 * Odwrócenie kierunku
	 */
	usint invertDir(usint);

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

			/**
			 * Aktualizacja drzewa!
			 */
			void subdive();

			void insertGroup(deque<Body*>*);
			void insert(Body*);

			void getBodiesAt(Rect<float>&, deque<Body*>&);
			void update(Rect<float>&);

			void clear();
			virtual void drawObject(Window*);

			~QuadTree();

		private:
			bool insertToSubQuad(Body*, bool);
	};
	
	class pEngine {
		public:
			enum {
				NONE,
				RIGHT,
				LEFT,
				UP,
				DOWN
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
			 * Obiekty buforowe!
			 */
			deque<Body*> list;

			/**
			 * Timer!
			 */
			bool pause;
			usint timer;
			usint sleep_time;

		public:
			pEngine(const Rect<float>&, float);

			/**
			 * Obszar widoczny!
			 */
			deque<Body*>* getVisibleBodies() {
				return &visible_bodies;
			}
			
			void setActiveRange(const Rect<float>& _active_range) {
				active_range = _active_range;
			}
			
			void insert(Body* body) {
				list.push_back(body);
				quadtree->insert(body);
			}

			void clear() {
				list.clear();
			}
			
			bool collide(const Body*, const Body*) const;
			bool moveAndCheck(float, float, Body*, const Body*);
			void updateWorld();

			void setSleep(usint);
			void setPause(bool _pause) {
				pause = _pause;
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
			// Czy warto sprawdzać dla niego kolizje
			bool isBodyActive(Body*);
			void checkCollisions(deque<Body*>&);

			usint checkVerticalCollision(Body*, Body*);
			usint checkHorizontalCollision(Body*, Body*);
	};
	
	/**
	 * Obiekt podlegający fizyce
	 */
	class Body: public Rect<float>, public Renderer {
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
				LADDER // drabina
			};

			/**
			 * State, stan obiektu: czy podlega fizyce?
			 */
			enum State {
				NONE = 1 << 0,
				STATIC = 1 << 1,
				HIDDEN = 1 << 2,
				BACKGROUND = 1 << 3, // tło nie oddziaływujące
				/**
				 * Element pomiędzy widocznym ekranem
				 * a widocznym musi nie podlegać fizyce
				 */
				BUFFERED = 1 << 4
			};

			usint state;

			Vector<float> velocity;
			float elasticity;
			float weight;

			/**
			 * Flaga, typ obiektu
			 */
			usint type;
			usint layer;
			usint script_id;

			/**
			 * Orientacja
			 */
			usint orientation;

			/**
			 * Długość życia ciała
			 */
			usint max_lifetime;
			usint lifetime;

			Body() :
							state(NONE),
							elasticity(0),
							weight(0),
							type(PLATFORM),
							layer(STATIC_LAYER),
							script_id(0),
							orientation(pEngine::UP),
							max_lifetime(0),
							lifetime(0) {
				x = 0;
				y = 0;
				w = 0;
				h = 0;
			}
			
			Body(float _x, float _y, float _w, float _h,
					float _elasticity = 1.f, float _weight = 1.f, usint _state =
							NONE) :
							state(_state),
							elasticity(_elasticity),
							weight(_weight),
							type(PLATFORM),
							layer(STATIC_LAYER),
							script_id(0),
							orientation(pEngine::UP),
							max_lifetime(0),
							lifetime(0) {
				x = _x;
				y = _y;
				w = _w;
				h = _h;
			}
			
			/**
			 * Długość życia do skasowania!
			 * Particle
			 */
			void setMaxLifetime(usint _max_lifetime) {
				max_lifetime = _max_lifetime;
				lifetime = 0;
				dynamically_allocated = true;
			}
			
			/**
			 * Wymiary!
			 */
			void setBounds(float _x, float _y, float _w, float _h) {
				x = _x;
				y = _y;
				w = _w;
				h = _h;
			}
			/**
			 * Nadawanie type!
			 */
			void setType(usint _type) {
				type = _type;
			}
			/**
			 * Nadawanie stanu!
			 */
			void setState(usint _state) {
				state = _state;
			}
			
			virtual void catchCollision(pEngine*, usint, Body*) {
			}
			
			virtual void drawObject(Window*) {
			}
			
			virtual ~Body() {
			}
	};
}

#endif /* PHYSICS_HPP_ */
