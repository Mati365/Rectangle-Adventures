/*
 * GUI.hpp
 *
 *  Created on: 11-05-2013
 *      Author: mateusz
 */

#ifndef GUI_HPP_
#define GUI_HPP_
#include "../Gameplay/Event.hpp"

#include "../Engine/Physics/Physics.hpp"
#include "../Engine/Graphics/Engine.hpp"
#include "../Engine/Graphics/Fonts.hpp"

#include "../Gameplay/Objects/Objects.hpp"

/**
 * Brak obsługi focusa! Ma to być tylko prosty
 * system!!
 */
namespace GUI {
	using namespace Engine;
	using namespace Physics;
	/**
	 * Myszka
	 */
	struct Mouse {
			Vector<int> pos;
			usint key;
	};
	extern Mouse mouse;
	extern bool isMouseCollision(float, float, float, float);
	
	/**
	 *
	 */
	class Control;
	class Callback {
		public:
			virtual void getCallback(Control* const &)=0;

			virtual ~Callback() {
			}
	};
	/**
	 *
	 */
	class Control: public Body, public EventListener {
		public:
			enum Position {
				VERTICAL,
				HORIZONTAL
			};

			enum State {
				NORMAL,
				CLICKED
			};

		protected:
			usint control_state;
			usint old_state;
			usint position;

			bool border_enabled;

			map<usint, Callback*> callbacks;

		public:
			Control(const Rect<float>& _bounds) :
							Body(_bounds.x, _bounds.y, _bounds.w, _bounds.h),
							control_state(NORMAL),
							old_state(NORMAL),
							position(VERTICAL),
							border_enabled(true) {
			}
			
			virtual void drawObject(Window*) = 0;
			virtual void catchEvent(const Event&);

			/**
			 * Nadawanie callbacku!
			 */
			void putCallback(usint, Callback*);

			/**
			 * Obwódka wokół kontrolki!
			 */
			void enableBorder(bool _border_enabled) {
				border_enabled = _border_enabled;
			}
			
			virtual ~Control() {
			}
	};
	/**
	 * Panel
	 */
	class Panel: public Control {
		protected:
			deque<Control*> objects;

		public:
			Panel(const Rect<float>& _bounds) :
							Control(_bounds) {
			}
			
			virtual void drawObject(Window*);
			virtual void catchEvent(const Event&);

			Control* operator[](usint index) const {
				return objects[index];
			}
			/**
			 * Dodawanie komponentów!
			 */
			void addControl(Control* control) {
				objects.push_back(control);
			}
			
			deque<Control*>* getObjects() {
				return &objects;
			}
	};
	
	/**
	 * Pasek postępu
	 */
	class ProgressBar: public Control {
		protected:
			Color col;
			Rect<float> obj;

			usint max_value;
			usint value;

		public:
			ProgressBar(const Rect<float>&, const Color&, usint, usint);
			virtual void drawObject(Window*);
			virtual ~ProgressBar() {
			}
			
			usint getValue() const {
				return value;
			}
			
			void setValue(usint _value) {
				value = _value;
			}
			
			void setColor(const Color& _col) {
				col = _col;
			}
			
			Color* getColor() {
				return &col;
			}
	};
	
	/**
	 * Przycisk
	 */
	class Button: public Control {
		protected:
			/** Tekst */
			glText text;

			/** Ikonka */
			IrregularPlatform* icon;

		public:
			Button(const Rect<float>&, const char*, PlatformShape* = NULL,
					bool = true, Callback* = NULL);

			virtual void drawObject(Window*);

			/** Pobieranie ikony */
			IrregularPlatform* getIcon() {
				return icon;
			}
			
			void setIcon(PlatformShape*);

			~Button() {
				if (icon) {
					delete icon;
				}
			}
	};
}

#endif /* GUI_HPP_ */
