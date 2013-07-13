/*
 * Event.hpp
 *
 *  Created on: 13-05-2013
 *      Author: mateusz
 */

#ifndef EVENT_HPP_
#define EVENT_HPP_
#include "../Engine/Graphics/Engine.hpp"

using namespace Engine;

class Event {
	public:
		enum {
			MOUSE_CLICKED, MOUSE_PRESSED, MOUSE_RELEASED, KEY_PRESSED
		};

		usint type;
		char key;

		Event(usint _type, char _key) :
				type(_type),
				key(_key) {
		}

		Event(usint _type) :
				type(_type),
				key(' ') {
		}
};

class EventListener {
	public:
		virtual void catchEvent(const Event&) = 0;

		virtual ~EventListener() {
		}
};

#endif /* EVENT_HPP_ */
