/*
 * Button.cpp
 *
 *  Created on: 25-06-2013
 *      Author: mateusz
 */
#include "GUI.hpp"

using namespace GUI;

Button::Button(const Rect<float>& _bounds, const char* _text,
		PlatformShape* _shape, bool _border_enabled, Callback* _callback) :
				Control(_bounds),
				text(oglWrapper::WHITE, _text, GLUT_BITMAP_HELVETICA_18, 18),
				icon(nullptr) {
	border_enabled = _border_enabled;
	if (_callback) {
		putCallback(Event::MOUSE_RELEASED, _callback);
	}
	setIcon(_shape);
}

/** Ikona na przycisku */
void Button::setIcon(PlatformShape* _shape) {
	if (_shape) {
		if (icon) {
			delete icon;
		}
		icon = new IrregularPlatform(x, y, pEngine::NONE, _shape, w);
		/** Skalowanie do wysokosci */
		while (icon->h > h - 4) {
			icon->fitToWidth(icon->w - 1.f);
		}
		/** Centrowanie */
		icon->x = x + w / 2 - icon->w / 2;
	}
}

/** Rysowanie */
void Button::drawObject(Window*) {
	switch (control_state) {
		case NORMAL:
			if (border_enabled) {
				glColor3f(1.f, 1.f, 1.f);

				glLineWidth(3.f);
				glBegin(GL_LINE_STRIP);

				glVertex2f(x, y);
				glVertex2f(x + w, y);
				glVertex2f(x + w, y + h);

				glEnd();

				oglWrapper::beginStroke(0xAAAA);
				glLineWidth(2.f);
				glBegin(GL_LINE_STRIP);

				glVertex2f(x + w, y + h);
				glVertex2f(x, y + h);
				glVertex2f(x, y);

				glEnd();
				oglWrapper::endStroke();
			}
			if (old_state != control_state) {
				text.setColor(oglWrapper::WHITE);
			}
			break;

			/**
			 *
			 */
		case CLICKED:
			oglWrapper::drawFillRect(x, y, w, h, oglWrapper::WHITE);
			if (old_state != control_state) {
				text.setColor(oglWrapper::BLACK);
			}
			break;
	}
	if (icon) {
		icon->drawObject(NULL);
	} else {
		text.printText(
				x + w / 2 - text.getScreenLength() / 2,
				y + h / 2 + text.getFontHeight() / 2 - 4);
	}
	//
	old_state = control_state;
}

