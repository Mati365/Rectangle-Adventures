/*
 * Lava.cpp
 *
 *  Created on: 08-08-2013
 *      Author: mateusz
 */
#include "Objects.hpp"

/**
 * Konstruktor:
 * - Wysokość jest na końcu mapy
 */
Lava::Lava(float _x, float _y) :
				Body(_x, _y, 0, 0) {
	/**
	 * Kształty płomieni
	 */
	flame_shapes[0] = getShapePointer("flame_1");
	flame_shapes[1] = getShapePointer("flame_2");
	//
	Rect<float>& bounds = flame_shapes[0]->getBounds();
	w = bounds.w;
	h = bounds.h;
}

/**
 * Odświeżanie pozycji
 */
void Lava::update() {
	
}

/**
 * Rysowanie lawy
 */
void Lava::drawObject(Window*) {
	update();
	//
}
