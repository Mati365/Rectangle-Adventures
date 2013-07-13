/*
 * Resources.hpp
 *
 *  Created on: 30-05-2013
 *      Author: mateusz
 */

#ifndef RESOURCES_HPP_
#define RESOURCES_HPP_
#include <GL/gl.h>

#include "../../Engine/Graphics/Engine.hpp"
#include "../../Engine/Physics/Physics.hpp"

#include "../Filesystem/Filesystem.hpp"

#include "ResourceManager.hpp"

using namespace Engine;
using namespace Physics;
using namespace Filesystem;

/////////////////////////// Główny system plików gry
#define FILESYSTEM_PACKAGE "filesystem.vfs"
#define FILESYSTEM_AUTHOR  "mateusz"

extern Package main_filesystem;
/**
 * Sprawdzenie autora pliku!!
 */
extern bool isFilesystemFake();

///////////////////////////

class MapINFO;

bool loadMap(const char*, MapINFO*);
MapINFO* loadMap(const char*);

///////////////////////////

class Platform;
class Character;
class PlatformShape;
class MapINFO: public Resource<usint> {
	public:
		pEngine* physics;

		/**
		 * Platform jest dużo, AllocKiller
		 * zwolnił by to niemiłosiernie
		 */
		deque<Platform*> objects;
		deque<usint> resources;

		/**
		 * Informacje początkowe o graczu
		 * i obiektach
		 */
		Rect<float> hero_bounds;
		Rect<float> bounds;

		MapINFO(const char*);

		Rect<float> getBounds() const {
			return bounds;
		}

		/**
		 * Elementy Resource'a
		 */
		virtual bool load(FILE*);
		virtual void unload();

		~MapINFO() {
			unload();
		}

	private:
		// Obliczanie wymiarów planszy
		void calcBounds();
};

/**
 * W następnym projekcie będzie VBO..
 */
class PlatformShape: public Resource<usint> {
	protected:
		GLuint id;
		Vector<float> bounds;

	public:
		PlatformShape(FILE*, const char*);

		/**
		 * Elementy Resource'a
		 */
		virtual bool load(FILE*);
		virtual void unload();

		/**
		 * Przeszukiwanie wierzchołków,
		 * wyznaczanie skrajnych rogów
		 */
		Vector<float>& getBounds() {
			return bounds;
		}

		GLuint getID() const {
			return id;
		}

		~PlatformShape();
};

#endif /* RESOURCES_HPP_ */
