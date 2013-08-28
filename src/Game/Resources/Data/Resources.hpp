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

/**
 * Ścieżki
 */
#define FILESYSTEM_PACKAGE "filesystem.vfs"
#define FILESYSTEM_AUTHOR  "mateusz"
#define SAVE_FILE_PATH "save.bin"

/**
 * Menedżery zasobów
 */
extern Package main_filesystem;
extern ResourceManager main_resource_manager;

/**
 * Dźwięki w grze
 */
enum Sounds {
	JUMP_SOUND,
	GUN_SHOT_SOUND,
	MENU_CHOOSE_SOUND,
	SCORE_SOUND,
	SPIKES_SOUND,
	EARTH_QUAKE_SOUND_1, // trzęsienie ziemi
	EARTH_QUAKE_SOUND_2, // trzęsienie ziemi
	DIE_SOUND
};

struct _Sound {
		Mix_Chunk* chunk;
		float volume;
};

extern _Sound sounds[];

void loadSoundsPack();
void unloadSoundsPack();

// Odtwarzanie dźwięku z ID
void playResourceSound(usint);

/**
 * Efekty w grze muszą być wczytane
 * przed pojawieniem się okna!
 */
enum Effect {
	WINDOW_SHADOW_SHADER,
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
bool loadMap(const char*, MapINFO*, usint = 0);
MapINFO* loadMap(const char*, usint = 0);

/**
 * Wczytywanie kształtu!
 */
PlatformShape* readShape(const string&, const char*, float);
PlatformShape* registerShape(FILE*, const char*, float);

PlatformShape* getShapePointer(const char*);
PlatformShape* getShapeFromFilesystem(const char*, float);

/**
 * Wczytywanie Mob'a
 */
bool readMob(FILE*);

///////////////////////////
class Platform;
class Character;
class MapINFO: public Resource {
	public:
		/**
		 * Konfiguracja wczytania mapy,
		 * po co w paralaxie mapie gracz?
		 */
		enum OpenConfig {
			WITHOUT_HERO = 1 << 1,
			WITHOUT_WEATHER = 1 << 2,
			WITHOUT_TEMPERATURE = 1 << 3
		};

		usint open_config;

		/** Fizyka */
		pEngine* physics;

		/**
		 * Platform jest dużo, AllocKiller
		 * zwolnił by to niemiłosiernie
		 */
		deque<usint> resources;

		/**
		 * Informacje początkowe o graczu
		 * i obiektach
		 */
		Rect<float> hero_bounds;
		Rect<float> bounds;

		/**
		 *  Kształt gracza
		 */
		PlatformShape* hero_shape;

		/**
		 * Ustawienia mapy
		 */
		usint map_weather;
		usint map_temperature;

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
		void readHeader(FILE*);
		void readShapes(FILE*);
		void readPlatforms(FILE*);
		void readMobsAndTriggers(FILE*);
};

/**
 * W następnym projekcie będzie VBO..
 */
class PlatformShape: public Resource {
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

		// Kolor domyślny
		Color main_col;

		// Szerokość lini
		usint line_width;

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
		 * Ustawienie grubości linii
		 */
		void setLineWidth(usint _line_width) {
			line_width = _line_width;
		}

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
		
		Color* getMainColor() {
			return &main_col;
		}

		~PlatformShape();

	protected:
		/**
		 * Obliczanie wymiarów
		 */
		void updateBounds();
};

/**
 * Zapis gry / wczytywane z systemu plików
 */
class Save: public PackagePointer {
	public:
		enum Stats {
			GAME_TIME,
			POINTS,
			REINCARNATIONS,
			LAST_LEVEL_INDEX
		};

		usint stats[LAST_LEVEL_INDEX + 1]; // Na którym lvl został

		Save() {
			clear();
		}

		virtual bool read(FILE*);
		virtual size_t write(FILE*);

		// Czyszczenie save
		void clear();

		virtual size_t getLength() {
			return sizeof(usint) * (LAST_LEVEL_INDEX + 1);
		}
};

/**
 * Menedżer zapisów
 * + Wzorzec singleton
 */
extern void exportSave(); // zapis save do systemu plików
extern void importSave(); // odczyt save z gry

class SaveManager {
	private:
		Save* save;
		char* file_name;

		/**
		 * Nazwa save musi być
		 */
		SaveManager(const char* _file_name) :
						file_name(Convert::getDynamicValue(_file_name)) {
			save = new Save;
			// Wczytywanie save

		}

		/**
		 * Destrukcja..
		 */
		~SaveManager() {
			if (file_name) {
				delete[] file_name;
			}
			delete save;
		}

	public:
		static SaveManager& getInstance() {
			static SaveManager _save_manager(SAVE_FILE_PATH);
			//
			return _save_manager;
		}

		/**
		 * Zapis save do systemu plików
		 */
		bool writeToFilesystem(Package*);
		bool readFromFilesystem(Package*);

		Save* getSave() {
			return save;
		}
};

#endif /* RESOURCES_HPP_ */
