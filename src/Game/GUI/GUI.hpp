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

/** Brak obslugi focusa! Ma to byc tylko prosty system!! */
namespace GUI {
	using namespace Engine;
	using namespace Physics;
	
	/** Myszka */
	struct Mouse {
			Vector<int> pos;
			usint key;
	};
	extern Mouse mouse;
	extern bool isMouseCollision(float, float, float, float);
	
	/** Callback */
	class Control;
	class Callback {
		public:
			virtual void getCallback(Control* const &)=0;

			virtual ~Callback() {
			}
	};
	
	/** Kontrolka */
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

			/** Odblokowanie obwodki */
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

			/** Nadawanie callbacku! */
			void putCallback(usint, Callback*);

			/** Obwodka wokol kontrolki! */
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
			
			Panel() :
							Control(Rect<float>(0, 0, 0, 0)) {
			}

			virtual void drawObject(Window*);
			virtual void catchEvent(const Event&);

			Control* operator[](usint index) const {
				return objects[index];
			}
			
			/** Dodawanie kontrolek */
			void addControl(Control* control) {
				objects.push_back(control);
			}
			
			deque<Control*>* getObjects() {
				return &objects;
			}
	};
	
	/** Pasek postepu */
	class ProgressBar: public Control {
		protected:
			Color col;
			Rect<float> obj;

			/** Wartosci */
			usint max_value;
			usint value;

		public:
			ProgressBar(const Rect<float>&, const Color&, usint, usint);

			virtual void drawObject(Window*);

			usint getValue() const {
				return value;
			}
			
			void setValue(usint _value) {
				value = _value;
			}
			
			void setMaxValue(usint _value) {
				max_value = _value;
			}

			void setColor(const Color& _col) {
				col = _col;
			}
			
			Color* getColor() {
				return &col;
			}
			
			virtual ~ProgressBar() {
			}
	};
	
	/** Guzik */
	class Button: public Control {
		public:
			enum Fillstyle {
				FILLED,
				SHARP
			};

		protected:
			/** Tekst */
			glText text;

			/** Ikonka */
			IrregularPlatform* icon;

			/** Czy tekst widoczny */
			bool text_visible;

			/** Odwrocona wklesnieta obwodka  */
			bool invert_border;

			/** Sposob zamalowania */
			usint fill_type;

		public:
			Button(const Rect<float>&, const char*, PlatformShape* = nullptr,
					bool = true, Callback* = nullptr);

			virtual void drawObject(Window*);

			/** Pobieranie ikony */
			IrregularPlatform* getIcon() {
				return icon;
			}
			
			void setIcon(PlatformShape*);

			~Button() {
				safe_delete<IrregularPlatform>(icon);
			}
	};
	
	/** Checkbox */
	class Checkbox: public Button {
		public:
			Checkbox(const Rect<float>&, const char*, PlatformShape* = nullptr,
					bool = true, Callback* = nullptr);

			void setSelected(bool _selected) {
				control_state = _selected ? CLICKED : NORMAL;
			}

			virtual void drawObject(Window*);
			virtual void catchEvent(const Event&);

			bool isChecked() const {
				return control_state == Control::CLICKED;
			}
	};

	/** Lista */
	class SelectList: public Control {
		protected:
			/** Elementy listy */
			deque<glText> elements;

			/** Zaznaczony item */
			string selected_item;

		public:
			SelectList(const Rect<float>&, Callback* = nullptr, string* =
					nullptr);

			virtual void drawObject(Window*);

			/** Event z okna */
			virtual void catchEvent(const Event&);

			/** Dodawanie do listy */
			void addListItem(const string& _str) {
				elements.push_back(
						glText(
								oglWrapper::WHITE,
								_str,
								GLUT_BITMAP_HELVETICA_18,
								18));
			}
			
			/** Zaznaczona */
			string getSelectedItem() const {
				return selected_item;
			}
	};
}

#endif /* GUI_HPP_ */
