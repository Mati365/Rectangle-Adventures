/*
 * Fonts.hpp
 *
 *  Created on: 24-02-2013
 *      Author: mati
 */

#ifndef FONTS_HPP_
#define FONTS_HPP_

#include <string>

#include "Engine.hpp"

#include <GL/glut.h>

#define NEWLINE_CHARACTER '/'

using namespace std;

#define CHARACTERS_READ_SPEED 3

class glText {
	private:
		Color col;
		string str;
		void* font;

		usint height;
		usint width;
		usint text_lines;

		bool anim;
		bool hidden;

		usint actual_anim_time;
		usint actual_text_pos;

		usint read_speed;

	public:
		glText(const Color&, const string&, void* = GLUT_BITMAP_TIMES_ROMAN_10,
				usint = 10);

		void setReadSpeed(usint _speed) {
			read_speed = _speed;
		}
		
		void setColor(const Color& _col) {
			col = _col;
		}
		
		void setHidden(bool _hidden) {
			hidden = _hidden;
		}
		
		bool isHidden() const {
			return hidden;
		}
		
		usint getFontHeight() const {
			return height;
		}
		
		usint getLinesOfText() const {
			return text_lines;
		}
		
		bool isAnim() const {
			return anim;
		}
		
		usint getRenderLength() const {
			return actual_text_pos;
		}
		
		string* getString() {
			return &str;
		}
		Color* getColor() {
			return &col;
		}
		
		/** Szerokosc tekstu w pixelach na monitorze! */
		usint getScreenLength();
		void updateScreenLength() {
			text_lines = 1;
			width = 0;
			for (usint i = 0; i < str.length(); ++i) {
				if (str[i] == NEWLINE_CHARACTER) {
					text_lines++;
				} else {
					width += glutBitmapWidth(font, str[i]);
				}
			}
		}
		
		/** Odblokowywanie animacji */
		void setAnim(bool);

		/** Dlugosc renderowania tekstu */
		void setRenderLength(usint);

		/** Ustawienie stringu */
		void setString(const string&, int);

		/** Wyswietlanie tekstu */
		void printText(float, float);
};

#endif /* FONTS_HPP_ */
