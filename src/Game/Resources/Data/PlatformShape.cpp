/*
 * PlatformShape.cpp
 *
 *  Created on: 01-06-2013
 *      Author: mateusz
 */
#include "Resources.hpp"

#include "../../Tools/Logger.hpp"

//---------------------------------------

PlatformShape* readShape(const string& _path, const char* _resource_label) {
	FILE* _file = main_filesystem.getExternalFile(_path.c_str(), NULL);
	if (!_file) {
		logEvent(Logger::LOG_ERROR,
				("Podany plik " + _path + " nie istnieje!").c_str());
		return NULL;
	}
	PlatformShape* shape = readShape(_file, _resource_label);
	//
	main_filesystem.closeExternalFile();
	return shape;
}

PlatformShape* readShape(FILE* _file, const char* _resource_label) {
	if (!_file) {
		logEvent(Logger::LOG_ERROR, "Podany obiekt nie istnieje!");
		return NULL;
	}
	PlatformShape* shape = new PlatformShape(_file, _resource_label);
	main_resource_manager.addResource(shape);
	return shape;
}
//---------------------------------------

PlatformShape::PlatformShape(FILE* _file, const char* _label) :
		Resource<usint>(_label),
		id(0),
		bounds(0, 0) {
	id = glGenLists(1);
	if (!id) {
		logEvent(Logger::LOG_ERROR, "Nie mogę zarejestrować tekstury!");
		return;
	}
	load(_file);
}

/**
 * Wczytywanie kształtu!
 */
bool PlatformShape::load(FILE* _file) {
	usint size = 0, line = 0;
	if (!_file) {
		return false;
	}
	unload();
	// Odczyt ilości wierzchołków!
	fscanf(_file, "%hu\n", &size);
	if (size == 0) {
		return false;
	}
	// Odczyt wraz z kompilacją listy!
	glNewList(id, GL_COMPILE);
	glLineWidth(2);
	glBegin (GL_LINE_STRIP);
	while (line != size) {
		char type = ' ';
		fscanf(_file, "%c", &type);
		/**
		 * Wczytywanie! 3 opcje:
		 * + [C]olor
		 * + [P]oint
		 * + [S]pace
		 */
		switch (type) {
			case 'C': {
				Color col;
				//---
				fscanf(_file, "%hu %hu %hu %hu\n", &col.r, &col.g, &col.b,
						&col.a);
				glColor4ub(col.r, col.g, col.b, col.a);
			}
				break;
				/**
				 *
				 */
			case 'P': {
				Vector<float> vec;
				//---
				fscanf(_file, "%f %f\n", &vec.x, &vec.y);
				glVertex2f(vec.x, vec.y);
				if (bounds.x < vec.x) {
					bounds.x = vec.x;
				}
				if (bounds.y < vec.y) {
					bounds.y = vec.y;
				}
			}
				break;
				/**
				 *
				 */
			case 'S':
				fscanf(_file, "\n");
				glEnd();
				glBegin(GL_LINE_STRIP);
				break;
		}
		line++;
	}
	glEnd();
	glEndList();
	//
	return true;
}

void PlatformShape::unload() {
	bounds.x = bounds.y = 0;
	glDeleteLists(id, 1);
}

PlatformShape::~PlatformShape() {
	unload();
}

