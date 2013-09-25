/*
 * Resources.hpp
 *
 *  Created on: 30-05-2013
 *      Author: mateusz
 */

#ifndef RESOURCES_HPP_
#define RESOURCES_HPP_
//#include <GL/gl.h>
#include <SDL/SDL_mixer.h>

#include "../../Engine/Graphics/Engine.hpp"
#include "../../Engine/Physics/Physics.hpp"

#include "../Filesystem/Filesystem.hpp"

#include "ResourceManager.hpp"

using namespace Engine;
using namespace Physics;
using namespace Filesystem;
using namespace oglWrapper;

/////////////////////////// Glowny system plikow

/** Sciezki */
#define FILESYSTEM_PACKAGE "filesystem.vfs"
#define FILESYSTEM_AUTHOR  "mateusz"

#define SAVE_FILE_RESPATH "save.bin"

/** Menedzer zasobow */
extern Package main_filesystem;
extern ResourceManager main_resource_manager;

/** Sprawdzenie autentycznosci pliku!! */
bool isFilesystemFake();

/**
 * Efekty w grze musza byc wczytane
 * przed pojawieniem sie okna!
 */
enum Effect {
	WINDOW_SHADOW_SHADER,
	HIT_CHARACTER_SHADER
};

extern Shader* shaders[];

void loadShadersPack();
void unloadShadersPack();

/**
 * Klasa przechowujaca cala mape
 * + Zwalnia ksztalty
 * + Zwalnia fizyka, ktora kasuje wytworzone
 * obiekty
 */
class Platform;
class Character;
class PlatformShape;
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
		 * Platform jest duzo, AllocKiller
		 * zwolnil� by to niemilosiernie
		 */
		struct MapResource {
				usint id;
				char label[256];

				MapResource(usint _id, const char* _label) :
								id(_id) {
					strcpy(label, _label);
				}
		};

		deque<MapResource> resources;

		/** Informacje poczatkowe o graczu i obiektach  */
		Rect<float> hero_bounds;
		Rect<float> bounds;

		/** Ksztalt gracza */
		PlatformShape* hero_shape;

		/** Ustawienia mapy */
		usint map_weather;
		usint map_temperature;

		MapINFO(const char*);

		Rect<float> getBounds() const {
			return bounds;
		}
		
		/** Elementy Resource'a */
		virtual bool load(FILE*);
		virtual void unload();

		~MapINFO() {
			unload();
		}
		
	private:
		/** Wczytywanie */
		void readHeader(FILE*);
		void readShapes(FILE*);
		void readPlatforms(FILE*);
		void readMobsAndTriggers(FILE*);
};

/** Wczytywanie mapy! */
bool loadMap(const char*, MapINFO*, usint = 0);
MapINFO* loadMap(const char*, usint = 0);

Body* readMob(FILE*);

/**
 * W nastepnym projekcie bedzie VBO..
 * Ksztalt platformy, przechowuje pozycje
 * vertex'ow w ksztalcie
 * + Kasuje je
 * + Usuwa liste
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

		/** Kąt obrotu! */
		float angle;

		/** Punkty */
		Point* points;
		usint count;

		/** Kolor domyslny */
		Color main_col;

		/** Szerokosc lini */
		usint line_width;

	public:
		PlatformShape(FILE*, const char*, float);

		/** Elementy Resource'a  */
		virtual bool load(FILE*);
		virtual void unload();

		/** Rekompilacja listy po transformacji! */
		bool recompile();
		void rotate(float);

		/** Ustawienie grubosci linii */
		void setLineWidth(usint _line_width) {
			line_width = _line_width;
			//
			recompile();
		}
		
		/**
		 * Przeszukiwanie wierzcholkow,
		 * wyznaczanie skrajnych rogow
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
		/** Obliczanie wymiarow */
		void updateBounds();
};

/** Wczytywanie ksztaltu! */
PlatformShape* readShape(const string&, const char*, float);
PlatformShape* registerShape(FILE*, const char*, float);

PlatformShape* getShapePointer(const char*);
PlatformShape* getShapeFromFilesystem(const char*, float);

/** Zapis gry / wczytywane z systemu plikow */
class Save: public PackagePointer {
	public:
		enum Stats {
			GAME_TIME,
			POINTS,
			REINCARNATIONS,
			LAST_LEVEL_INDEX
		};

		usint stats[LAST_LEVEL_INDEX + 1]; // Na ktorym lvl zostal
		
		Save() {
			clear();
		}
		
		virtual bool read(FILE*);
		virtual size_t write(FILE*);

		/** Czyszczenie save */
		void clear();

		virtual size_t getLength() {
			return sizeof(usint) * (LAST_LEVEL_INDEX + 1);
		}
};

/**
 * Menedzer zapisow
 * + Wzorzec singleton
 */
extern void exportSave(); // zapis save do systemu plikow
extern void importSave(); // odczyt save z gry

class SaveManager {
	private:
		Save* save;
		char* file_name;

		/** Nazwa save musi byc */
		SaveManager(const char* _file_name) :
						file_name(Convert::getDynamicValue(_file_name)) {
			save = new Save;
		}
		
	public:
		static SaveManager& getInstance() {
			static SaveManager _save_manager(SAVE_FILE_RESPATH);
			//
			return _save_manager;
		}
		
		/** Zapis save do systemu plikow */
		bool writeToFilesystem(Package*);
		bool readFromFilesystem(Package*);

		Save* getSave() {
			return save;
		}
		
		/** Destrukcja.. */
		~SaveManager() {
			if (file_name) {
				delete[] file_name;
			}
			delete save;
		}
};

#endif /* RESOURCES_HPP_ */
