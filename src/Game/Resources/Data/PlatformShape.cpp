/*
 * PlatformShape.cpp
 *
 *  Created on: 01-06-2013
 *      Author: mateusz
 */
#include "Resources.hpp"

#include "../../Tools/Logger.hpp"

//---------------------------------------

PlatformShape* readShape(const string& _path, const char* _resource_label,
		float _angle) {
	FILE* _file = main_filesystem.getExternalFile(_path.c_str(), NULL);
	if (!_file) {
		logEvent(
				Logger::LOG_ERROR,
				("Podana tekstura" + _path + " nie istnieje!").c_str());
		return NULL;
	}
	PlatformShape* shape = readShape(_file, _resource_label, _angle);
	//
	main_filesystem.closeExternalFile();
	return shape;
}

PlatformShape* readShape(FILE* _file, const char* _resource_label,
		float _angle) {
	if (!_file) {
		logEvent(
				Logger::LOG_ERROR,
				("Podana tekstura " + (string )_resource_label
						+ " nie istnieje!").c_str());
		return NULL;
	}
	PlatformShape* shape = new PlatformShape(_file, _resource_label, _angle);
	main_resource_manager.addResource(shape);
	return shape;
}

PlatformShape* getShapePointer(const char* _label) {
	return dynamic_cast<PlatformShape*>(main_resource_manager.getByLabel(_label));
}

//---------------------------------------

PlatformShape::PlatformShape(FILE* _file, const char* _label, float _angle) :
				Resource<usint>(_label),
				id(0),
				bounds(0, 0, 0, 0),
				angle(TO_RAD(_angle)),
				//
				points(NULL),
				count(0) {
	id = glGenLists(1);
	if (!id) {
		logEvent(Logger::LOG_ERROR, "Nie mogę zarejestrować tekstury!");
		return;
	}
	load(_file);
}

/**
 * Odświeżanie wymiarów po np. obrocie
 */
void PlatformShape::updateBounds() {
	bounds.w = bounds.h = 0;
	//
	for (usint i = 0; i < count; ++i) {
		Vector<float>& vec = points[i].pos;
		// Wymiary
		if (bounds.w < vec.x) {
			bounds.w = vec.x;
		}
		if (bounds.h < vec.y) {
			bounds.h = vec.y;
		}
		if (bounds.x > vec.x) {
			bounds.x = vec.x;
		}
		if (bounds.y > vec.y) {
			bounds.y = vec.y;
		}
	}
}

/**
 * Wczytywanie kształtu!
 *
 * Wzór na obrót:
 * x' = x * cos(angle) - y * sin(angle)
 * y' = x * sin(angle) + y * cos(angle)
 */
bool PlatformShape::load(FILE* _file) {
	if (!_file) {
		return false;
	}
	/**
	 *
	 */
	unload();
	
	// Odczyt ilości wierzchołków!
	usint line = 0;
	fscanf(_file, "%hu\n", &count);
	if (count == 0) {
		return false;
	}
	points = new Point[count];
	
	// Odczytywanie wielkości obiektu!
	while (line != count) {
		char type = ' ';
		fscanf(_file, "%c", &type);
		switch (type) {
			/**
			 *
			 */
			case 'P': {
				Vector<float> vec;
				//---
				fscanf(_file, "%f %f\n", &vec.x, &vec.y);
				points[line] = {type, oglWrapper::WHITE, vec};
			}
			break;

			case 'C': {
				Color col;
				//---
				fscanf(
						_file,
						"%hu %hu %hu %hu\n",
						&col.r,
						&col.g,
						&col.b,
						&col.a);
				points[line] = {type, col};
			}
			break;

			/**
			 *
			 */
			case 'S':
			fscanf(_file, "\n");
			//---
			points[line] = {type};
			break;
		}
		line++;
	}
	updateBounds();
	rotate(angle);
	/**
	 * Rekompilacja listy!
	 */
	return recompile();
}

/**
 * Kompilacja listy!
 */
bool PlatformShape::recompile() {
	if (!points || count == 0) {
		return false;
	}
	glDeleteLists(id, 1);
	
	// Kompilacja!
	glNewList(id, GL_COMPILE);
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
	for (usint i = 0; i < count; ++i) {
		Point* point = &points[i];
		/**
		 * Wczytywanie! 3 opcje:
		 * + [C]olor
		 * + [P]oint
		 * + [S]pace
		 */
		switch (point->type) {
			case 'C':
				glColor4ub(
						point->col.r,
						point->col.g,
						point->col.b,
						point->col.a);
				break;
				
				/**
				 *
				 */
			case 'P': {
				Vector<float>* pos = &point->pos;
				//
				glVertex2f(pos->x, pos->y);
			}
				break;
				
				/**
				 *
				 */
			case 'S':
				glEnd();
				glBegin(GL_LINE_STRIP);
				break;
		}
	}
	glEnd();
	glEndList();
	return true;
}

/**
 * Rotacja wokół punktu
 */
void PlatformShape::rotate(float _angle) {
	angle = _angle;
	//
	bounds.x = bounds.y = 0;
	for (usint i = 0; i < count; ++i) {
		Vector<float>* pos = &points[i].pos;
		Vector<float> new_pos;
		//
		new_pos.x = (pos->x - bounds.w / 2) * cosf(angle)
				- (pos->y - bounds.h / 2) * sinf(angle) + bounds.w / 2;
		new_pos.y = (pos->x - bounds.w / 2) * sinf(angle)
				+ (pos->y - bounds.h / 2) * cosf(angle) + bounds.h / 2;
		//
		(*pos) = new_pos;
		//
		if (bounds.x > pos->x) {
			bounds.x = pos->x;
		}
		if (bounds.y > pos->y) {
			bounds.y = pos->y;
		}
	}
	/**
	 * Po rotacji wyrównanie punktu do pozycji
	 */
	for (usint i = 0; i < count; ++i) {
		Vector<float>* pos = &points[i].pos;
		//
		pos->x -= bounds.x < 0 ? bounds.x : -bounds.x;
		pos->y -= bounds.y < 0 ? bounds.y : -bounds.y;
	}
	updateBounds();
	/**
	 * Rekompilacja listy
	 */
	recompile();
}

void PlatformShape::unload() {
	if (points) {
		delete[] points;
	}
	bounds.x = bounds.y = bounds.w = bounds.h = 0;
	glDeleteLists(id, 1);
}

PlatformShape::~PlatformShape() {
	unload();
}

