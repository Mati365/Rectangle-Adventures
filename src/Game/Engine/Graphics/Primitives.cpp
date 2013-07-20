/*
 * OglWrapper.hpp
 *
 *  Created on: 23-02-2013
 *      Author: mati
 */
#include <GL/glut.h>
#include <math.h>

#include "Engine.hpp"

/**
 * Podstawowe kolory!
 */
Color oglWrapper::RED(255, 0, 0);
Color oglWrapper::DARK_RED(128, 0, 0);
Color oglWrapper::GREEN(124, 225, 24);
Color oglWrapper::DARK_GREEN(0, 128, 0);
Color oglWrapper::BLUE(0, 0, 255);
Color oglWrapper::DARK_BLUE(128, 0, 0);
Color oglWrapper::BLACK(0, 0, 0);
Color oglWrapper::WHITE(255, 255, 255);
Color oglWrapper::GRAY(128, 128, 128);
Color oglWrapper::YELLOW(255, 255, 0);
Color oglWrapper::ORANGE(255, 165, 0);
Color oglWrapper::PURPLE(191, 0, 255);

void oglWrapper::drawRect(float x, float y, float w, float h, const Color& col,
	float stroke) {
	glColor4ub(col.r, col.g, col.b, col.a);
	glLineWidth(stroke);
	glBegin (GL_LINE_LOOP);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glVertex2f(x, y);
	glEnd();
}

void oglWrapper::drawFillRect(float x, float y, float w, float h,
	const Color& col) {
	glColor4ub(col.r, col.g, col.b, col.a);
	glLineWidth(1);
	glBegin (GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}

void oglWrapper::drawCircle(float x, float y, float r, float spacing,
	const Color& col, float stroke) {
	float cx, cy, theta;
	glColor4ub(col.r, col.g, col.b, col.a);
	glLineWidth(stroke);
	glBegin (GL_LINE_LOOP);
	for (usint i = 0; i < spacing; ++i) {
		theta = 2.0f * 3.1415926f * float(i) / float(spacing);
		cx = r * cosf(theta);
		cy = r * sinf(theta);

		glVertex2f(x + cx, y + cy);
	}
	glEnd();
}

void oglWrapper::drawLine(float x, float y, float tx, float ty,
	const Color& col, float stroke) {
	glColor4ub(col.r, col.g, col.b, col.a);
	glLineWidth(stroke);
	glBegin (GL_LINES);
	glVertex2f(x, y);
	glVertex2f(tx, ty);
	glEnd();
}

void oglWrapper::drawTriangle(float x, float y, float w, float h,
	const Color& col, float stroke) {
	glColor4ub(col.r, col.g, col.b, col.a);
	glLineWidth(stroke);
	glBegin (GL_LINE_LOOP);
	glVertex2f(x, y + h);
	glVertex2f(x + w, y + h);
	glVertex2f(x + w / 2, y);
	glVertex2f(x, y + h);
	glEnd();
}
