/*
 * Fonts.cpp
 *
 *  Created on: 02-03-2013
 *      Author: mati
 */
#include "Fonts.hpp"

glText::glText(const Color& _col, const string& _str, void* _font =
GLUT_BITMAP_TIMES_ROMAN_10, usint _height = 10) :
		col(_col),
		str(_str),
		font(_font),
		height(_height),
		text_lines(1),
		anim(false),
		hidden(false),
		actual_anim_time(0),
		actual_text_pos(0) {
	updateScreenLength();
}

void glText::setAnim(bool _anim) {
	actual_text_pos = actual_anim_time = 0;
	anim = _anim;
}

void glText::setRenderLength(usint _length) {
	actual_text_pos = _length;
}

void glText::setString(const string& _str, int _render_length) {
	str = _str;
	actual_text_pos = 0;
	updateScreenLength();
	if (_render_length != -1) {
		anim = true;
	} else {
		anim = false;
	}
}

usint glText::getScreenLength() {
	return width;
}

void glText::printText(float _x, float _y) {
	float _x_buf = _x;
	if (hidden) {
		return;
	}
	if (anim) {
		actual_anim_time++;
		if (actual_anim_time > CHARACTERS_READ_SPEED) {
			actual_anim_time = 0;
			actual_text_pos++;
			if (actual_text_pos > str.length()) {
				anim = false;
				actual_text_pos = str.length();
			}
		}
	}

	glColor4ub(col.r, col.g, col.b, col.a);
	glRasterPos2f(_x, _y);
	for (usint i = 0; i < (!anim ?
			str.length() : actual_text_pos); ++i) {
		if (str[i] == NEWLINE_CHARACTER) {
			_y += height * 1.5f;
			_x = _x_buf;
			glRasterPos2f(_x, _y);
		} else {
			glutBitmapCharacter(font, str[i]);
		}
	}
}
