/*
 * Resources.hpp
 *
 *  Created on: 30-05-2013
 *      Author: mateusz
 */

#ifndef RESOURCES_HPP_
#define RESOURCES_HPP_
#include <GL/gl.h>
#include <SDL/SDL_mixer.h>

#include "../../Engine/Graphics/Engine.hpp"
#include "../../Engine/Physics/Physics.hpp"

#include "../Filesystem/Filesystem.hpp"

#include "ResourceManager.hpp"

using namespace Engine;
using namespace Physics;
using namespace Filesystem;
using namespace oglWrapper;

/////////////////////////// Główny system plików gry

#define FILESYSTEM_PACKAGE "filesystem.vfs"
#define FILESYSTEM_AUTHOR  "mateusz"

extern Package main_filesystem;
extern ResourceManager<usint> main_resource_manager;

/**
 * Dźwięki w grze
 */
enum Sounds {
	JUMP_SOUND,
	GUN_SHOT_SOUND,
	MENU_CHOOSE_SOUND,
	SCORE_SOUND,
	SPIKES_SOUND,
	DIE_SOUND
};

struct _Sound {
		Mix_Chunk* chunk;
		float volume;
};

extern _Sound sounds[];

void loadSoundsPack();
void unloadSoundsPack();

/**
 * Efekty w grze muszą być wczytane
 * przed pojawieniem się okna!
 */
enum Effect {
	WINDOW_SHADOW_SHADER,
	WINDOW_DEATH_SHADER,
	HIT_CHARACTER_SHADER
};

extern Shader* shaders[];

void loadShadersPack();
void unloadShadersPack();

/**
 * Sprawdzenie autora pliku!!
 */
bool isFilesystemFake();

///////////////////////////

class MapINFO;
class PlatformShape;
/**
 * Wczytywanie mapy!
 */
bool loadMap(const char*, MapINFO*);
MapINFO* loadMap(const char*);

/**
 * Wczytywanie kształtu!
 */
PlatformShape* readShape(const string&, const char*, float);
PlatformShape* readShape(FILE*, const char*, float);

PlatformShape* getShapePointer(const char*);

/**
 * Wczytywanie Mob'a
 */
bool readMob(FILE*);

///////////////////////////
class Platform;
class Character;
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
		struct Point {
				char type;
				Color col;
				Vector<float> pos;
		};

	protected:
		GLuint id;
		Rect<float> bounds;

		// Kąt obrotu!
		float angle;

		// Punkty
		Point* points;
		usint count;

	public:
		PlatformShape(FILE*, const char*, float);

		/**
		 * Elementy Resource'a
		 */
		virtual bool load(FILE*);
		virtual void unload();

		/**
		 * Rekompilacja listy po transformacji!
		 */
		bool recompile();
		void rotate(float);

		/**
		 * Przeszukiwanie wierzchołków,
		 * wyznaczanie skrajnych rogów
		 */
		Rect<float>& getBounds() {
			return bounds;
		}
		
		GLuint getID() const {
			return id;
		}
		
		~PlatformShape();

	protected:
		/**
		 * Obliczanie wymiarów
		 */
		void updateBounds();
};

#endif /* RESOURCES_HPP_ */
