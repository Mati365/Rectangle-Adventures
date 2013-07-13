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

#define STATIC_LAYER 0
#define MAX_LAYER 3

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

			operator Vector<float>() {
				return Vector<float>(x, y);
			}
	};

	class pEngine;
	class Body: public Rect<float>, public Renderer {
		public:
			/**
			 * Flaga obiektu, czym on jest?
			 */
			enum Flag {
				HERO, ENEMY, PLATFORM, SCORE, BULLET
			};

			/**
			 * State, stan obiektu: czy podlega fizyce?
			 */
			enum State {
				NONE = 0x01, STATIC = 0x02, HIDDEN = 0x04
			};

			usint state;

			Vector<float> velocity;
			float elasticity;
			float weight;

			/**
			 * Flaga, typ obiektu
			 */
			usint flag;
			usint layer;

			Body() :
					state(NONE),
					elasticity(0),
					weight(0),
					flag(PLATFORM),
					layer(STATIC_LAYER) {
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
					flag(PLATFORM),
					layer(STATIC_LAYER) {
				x = _x;
				y = _y;
				w = _w;
				h = _h;
			}

			void setBounds(float _x, float _y, float _w, float _h) {
				x = _x;
				y = _y;
				w = _w;
				h = _h;
			}
			/**
			 * Nadawanie flagi!
			 */
			void setFlag(usint _flag) {
				flag = _flag;
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

	class QuadTree: public Renderer {
		private:
			Rect<float> rect;
			deque<Body*> bodies;
			usint level;
			/**
			 * STAŁE
			 */
			QuadTree* NW;
			QuadTree* NE;
			QuadTree* SW;
			QuadTree* SE;

		public:
			QuadTree(const Rect<float>&, usint, usint);

			void insert(deque<Body*>*);
			void insert(Body*);
			void getBodiesAt(const Rect<float>&, deque<Body*>*);
			void getLowestElements(deque<deque<Body*> >&);

			void clear();
			virtual void drawObject(Window*);

			~QuadTree();

		private:
			void insertToSubQuad(Body*);
			bool containsObject(const Rect<float>*, const Rect<float>*);
	};

	class pEngine {
		public:
			enum {
				NONE, RIGHT, LEFT, UP, DOWN
			};

		private:
			QuadTree* quadtree;
			Rect<float> bounds;
			float gravity_speed;
			/**
			 * Obiekty buforowe!
			 */
			deque<Body*> list;
			deque<Body*> to_remove;

			deque<deque<Body*> > bodies;
			/**
			 * Timer!
			 */
			bool pause;
			usint timer;
			usint sleep_time;

		public:
			pEngine(const Rect<float>&, float);

			void insert(Body* body) {
				list.push_back(body);
			}
			void remove(Body* body) {
				to_remove.push_back(body);
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

			~pEngine();

		private:
			void checkCollisions(deque<Body*>&);

			usint checkVerticalCollision(Body*, Body*);
			usint checkHorizontalCollision(Body*, Body*);
	};
}

#endif /* PHYSICS_HPP_ */
