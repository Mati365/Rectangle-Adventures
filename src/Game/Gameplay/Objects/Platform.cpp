/*
 * Platform.cpp
 *
 *  Created on: 12-03-2013
 *      Author: mati
 */
#include <cmath>
#include <string>

#include "Objects.hpp"

#include "../../Tools/Logger.hpp"

/**
 * Konstruktor
 */
Platform::Platform(float _x, float _y, float _w, float _h, const Color& _col,
		usint _state) :
				Body(_x, _y, _w, _h),
				col(_col),
				repeat_movement(true),
				fill_type(SIMPLE),
				list(glGenLists(1)) {
	state = _state;
	type = PLATFORM;
	//
	for (auto& obj : border) {
		obj = true;
	}
}

/**
 * Odświeżanie ruchu platformy
 */
bool Platform::updatePlatform() {
	if (max_distance.x != 0 || max_distance.y != 0) {
		{
			float x = abs(distance.x), y = abs(distance.y), to_x = abs(
					max_distance.x), to_y = abs(max_distance.y);
			if ((x > 0 && x >= to_x) || (y > 0 && y >= to_y)) {
				if (!repeat_movement) {
					velocity.x = velocity.y = 0;
					return true;
				} else {
					distance.x = distance.y = 0;
					velocity.x = -velocity.x;
					velocity.y = -velocity.y;
				}
			}
		}
		x += velocity.x;
		y += velocity.y;
		//
		distance.x += distance.x < max_distance.x ? velocity.x : -velocity.x;
		distance.y += distance.y < max_distance.y ? velocity.y : -velocity.y;
		return true;
	}
	return false;
}

/**
 * Wyłączenie poruszania się!
 */
void Platform::disableMoving() {
	max_distance.x = max_distance.y = 0;
}

/**
 * Ścieżka poruszania się
 */
void Platform::setMovingDir(const Vector<float>& _velocity,
		const Vector<float>& _distance, bool _repeat_movement) {
	velocity = _velocity;
	max_distance = _distance;
	repeat_movement = _repeat_movement;
	distance.x = distance.y = 0;
}

/**
 * Typ zamalowania
 */
void Platform::setFillType(usint _fill_type) {
	fill_type = _fill_type;
	
	// Tarcie dla lodowej
	if (fill_type == ICY) {
		roughness = 0.94f;
	}
}

/**
 * Ustawienie obramowania!
 */
void Platform::setBorder(bool top, bool right, bool down, bool left) {
	border[pEngine::UP - 1] = top;
	border[pEngine::RIGHT - 1] = right;
	border[pEngine::DOWN - 1] = down;
	border[pEngine::LEFT - 1] = left;
}

void Platform::drawBorder() {
	bool gradient = (fill_type != Type::NONE);
	
	/**
	 * Obramowanie!
	 */
	glColor4ub(col.r, col.g, col.b, col.a);
	glLineWidth(gradient ? 4 : 3);
	
	if (border[0] && border[1] && border[2] && border[3]) {
		glBegin(GL_LINE_LOOP);
		
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		
		if (gradient) {
			glColor4ub(col.r, col.g, col.b, 0.f);
		}
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		
		glEnd();
	} else {
		glBegin(GL_LINES);
		glColor4ub(col.r, col.g, col.b, col.a);
		
		if (border[pEngine::UP - 1]) {
			glVertex2f(x, y);
			glVertex2f(x + w, y);
			
		} else if (border[pEngine::DOWN - 1]) {
			glVertex2f(x, y + h);
			glVertex2f(x + w, y + h);
		}
		
		if (border[pEngine::LEFT - 1]) {
			glColor4ub(col.r, col.g, col.b, col.a);
			glVertex2f(x, y);
			
			if (gradient) {
				glColor4ub(col.r, col.g, col.b, 0.f);
			}
			glVertex2f(x, y + h);
		}
		if (border[pEngine::RIGHT - 1]) {
			glColor4ub(col.r, col.g, col.b, col.a);
			glVertex2f(x + w, y);
			
			if (gradient) {
				glColor4ub(col.r, col.g, col.b, 0.f);
			}
			glVertex2f(x + w, y + h);
		}
		
		glEnd();
	}
	/**
	 * Śnieg pokrywający platformę
	 * w zależności od jej typu
	 */
	if (fill_type == ICY) {
		glLineWidth(5);
		glBegin(GL_LINE_STRIP);
		
		glColor4ub(255, 255, 255, 55);
		glVertex2f(x - 2, y + h * 0.4f);
		
		glColor4ub(255, 255, 255, 255);
		glVertex2f(x - 2, y - 2);
		glVertex2f(x + w + 2, y - 2);
		
		glColor4ub(255, 255, 255, 55);
		glVertex2f(x + w + 2, y + h * 0.4f);
		
		glEnd();
	}
}

/**
 * Rysowanie wypełnienia!
 */
void Platform::drawBody() {
	if (fill_type == Type::NONE) {
		return;
	}
	float line_stroke = 2.f;
	
	/**
	 * Wypełnienie!
	 */
	glBegin(GL_QUADS);
	
	glColor4ub(col.r * 0.3f, col.g * 0.3f, col.b * 0.3f, col.a);
	glVertex2f(x + line_stroke, y + line_stroke);
	glVertex2f(x + w - line_stroke, y + line_stroke);
	
	glColor4ub(col.r * 0.3f, col.g * 0.3f, col.b * 0.3f, col.a * 0.1f);
	glVertex2f(x + w - line_stroke, y + h - line_stroke);
	glVertex2f(x + line_stroke, y + h - line_stroke);
	
	glEnd();
	
	/**
	 * Kreski w środku
	 */
	glLineWidth(line_stroke + 1);
	switch (fill_type) {
		
		/**
		 *  Proste!
		 */
		case SIMPLE:
		case ICY:
			glBegin(GL_LINES);
			
			if (w > h) {
				for (usint i = 0; i < w / 10; ++i) {
					glColor4ub(col.r, col.g, col.b, 0.f);
					glVertex2f(x + i * 10, y + h);
					
					glColor4ub(col.r, col.g, col.b, col.a * 0.3f);
					glVertex2f(x + i * 10, y);
				}
			} else {
				for (usint i = 0; i < h / 10; ++i) {
					float proc = (float) i / (h / 10.f);
					//
					glColor4ub(
							col.r,
							col.g,
							col.b,
							col.a * 0.2f * (1.f - proc));
					glVertex2f(x + w, y + i * 10);
					glVertex2f(x, y + i * 10);
				}
			}
			
			glEnd();
			break;
			
			/**
			 *  Ukośne!
			 */
		case DIAGONAL:
			glBegin(GL_LINES);
			
			if (w > h) {
				for (usint i = 0; i < w / 10; ++i) {
					glColor4ub(col.r, col.g, col.b, 0.f);
					glVertex2f(x + i * 10, y + h);
					
					glColor4ub(col.r, col.g, col.b, col.a * 0.5f);
					glVertex2f(x + i * 10 + 10, y);
				}
			} else {
				for (usint i = 0; i < h / 10; ++i) {
					glColor4ub(col.r, col.g, col.b, 0.f);
					glVertex2f(x + w, y + i * 10);
					
					glColor4ub(col.r, col.g, col.b, col.a * 0.3f);
					glVertex2f(x, y + i * 10 + 10);
				}
			}
			
			glEnd();
			break;
			
			/**
			 *  Stal jak w Donkey Kong
			 */
		case METAL:
			glBegin(GL_LINES);
			
			if (w > h) {
				for (usint i = 0; i < w / 8; ++i) {
					int space = 8;
					if (i % 2) {
						space = -space;
					}
					
					glColor4ub(col.r, col.g, col.b, 0.f);
					glVertex2f(x + i * 8, y + h);
					
					glColor4ub(col.r, col.g, col.b, col.a * 0.3f);
					glVertex2f(x + i * 8 + space, y);
				}
			} else {
				for (usint i = 0; i < h / 10; ++i) {
					glColor4ub(col.r, col.g, col.b, 0.f);
					glVertex2f(x + w, y + i * 10);
					
					glColor4ub(col.r, col.g, col.b, col.a * 0.3f);
					glVertex2f(x, y + i * 10 + 10);
				}
			}
			
			glEnd();
			break;
			
			/**
			 *  Alpha w środku!
			 */
		case FILLED:
			oglWrapper::drawFillRect(x, y, w, h, col);
			break;
	}
}

/**
 * Aby zoptymalizować wyświetlanie statycznych
 * obiektów muszą być listy!
 */
void Platform::compileList() {
	glDeleteLists(list, 1);
	glNewList(list, GL_COMPILE);
	
	drawBorder();
	drawBody();
	
	glEndList();
}

void Platform::drawObject(Window*) {
	/**
	 * oglWrapper nie wykorzystywany!
	 * Mała wydajność!
	 *
	 */
	if (updatePlatform() || !IS_SET(state, STATIC)) {
		drawBorder();
		drawBody();
	} else {
		/**
		 * Szybsze dla statycznych platform!
		 */
		glCallList(list);
	}
}
//---------------------------------------

IrregularPlatform::IrregularPlatform(float _x, float _y, usint _state,
		PlatformShape* _shape, float _width) :
				Platform(_x, _y, 0, 0, oglWrapper::WHITE, _state),
				shape(NULL),
				scale(1) {
	if (!_shape) {
		return;
	}
	setShape(_shape);
	if (_width != -1) {
		fitToWidth(_width);
	}
}

void IrregularPlatform::setShape(PlatformShape* _shape) {
	if (!_shape) {
		return;
	}
	Rect<float>& _bounds = _shape->getBounds();
	//
	w = _bounds.w;
	h = _bounds.h;
	shape = _shape;
}

/**
 * Dostosuj proporcje do szerokości!
 */
void IrregularPlatform::setScale(float _scale) {
	if (!shape) {
		return;
	}
	scale = _scale;
	w = shape->getBounds().w * scale;
	h = shape->getBounds().h * scale;
}

void IrregularPlatform::fitToWidth(float _w) {
	if (!shape) {
		return;
	}
	setScale(_w / shape->getBounds().w);
}

/**
 * Narysuj!
 */
void IrregularPlatform::drawObject(Window*) {
	/**
	 * Odświeżanie kształtu - mógła zostać podmieniona
	 * temperatura!
	 */
	updatePlatform();
	
	/**
	 * Rysowanie
	 */
	glPushMatrix();
	glTranslatef(x, y, 1);
	
	if (scale != 1.f) {
		glScalef(scale, scale, 1.f);
	}
	if (shape) {
		glColor4ub(col.r, col.g, col.b, col.a);
		glCallList(shape->getID());
	}
	
	glPopMatrix();
	//oglWrapper::drawRect(x, y, w, h, oglWrapper::GREEN, 2.f);
}
