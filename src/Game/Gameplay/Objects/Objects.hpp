/*
 * Objects.hpp
 *
 *  Created on: 02-03-2013
 *      Author: mati
 */

#ifndef OBJECTS_HPP_
#define OBJECTS_HPP_
#include <map>

#include "../../Engine/Graphics/Engine.hpp"
#include "../../Engine/Graphics/Fonts.hpp"
#include "../../Engine/Physics/Physics.hpp"
#include "../../Engine/Sound/Sounds.hpp"

#include "../../Tools/Tools.hpp"

#include "../../Resources/Data/ResourceManager.hpp"
#include "../../Resources/Data/Resources.hpp"

#include "../Script/Script.hpp"

using namespace Engine;
using namespace Physics;
using namespace Sound;

/**
 * Konfiguracja gameplay!
 */
#define MAX_LIVES 3
#define MAX_SCORE 50
#define DEATH -1

#define DEFAULT_LEVITATION_DURATION 120

/**
 * Platforma, po której porusza się gracz
 */
class Platform: public Body, public Cloneable {
	public:
		enum Type {
			DIAGONAL, // ukośne linie
			SIMPLE, // proste linie
			FILLED, // zamalowana
			ICY, // oblodzona + SIMPLE
			NONE
		};

	protected:
		Color col;
		/**
		 * Dla ruchomych elementów..
		 */
		Vector<float> distance;
		Vector<float> max_distance;
		/**
		 * Powtarzalność ruchów
		 */
		bool repeat_movement;

		usint fill_type;
		/**
		 * Dla przyśpieszenia renderingu!
		 */
		usint list;
		/**
		 * Obramowanie platformy
		 */
		bool border[4];

	public:
		Platform(float, float, float, float, const Color&, usint);

		virtual void drawObject(Window*);
		virtual void catchCollision(pEngine*, usint, Body*) {
		}

		/**
		 * Odblokowanie poruszania się!
		 */
		void setMovingDir(const Vector<float>&, const Vector<float>&, bool);
		void disableMoving();

		// Argumenty zgodnie z ruchem wskazówek zegara
		void setBorder(bool, bool, bool, bool);
		void setFillType(usint);

		/**
		 * Kompilacja listy dla statycznej!
		 */
		void compileList();

		/**
		 * Typy platformy pod względem
		 * rysowania!
		 */
		usint setOrientation() const {
			return orientation;
		}

		usint getFillType() const {
			return fill_type;
		}
		Color* getColor() {
			return &col;
		}

		/**
		 * Klonowanie obiektów!
		 */
		virtual Cloneable* getClone() const {
			return new Platform(*this);
		}

		virtual bool recover(Cloneable* _clone) {
			Platform* obj = dynamic_cast<Platform*>(_clone);
			if (!obj) {
				return false;
			}
			(*this) = *obj;
			return true;
		}

		~Platform() {
			if (list) {
				glDeleteLists(list, 1);
			}
		}

	protected:
		bool updatePlatform();
		/**
		 *  Rysowanie!
		 */
		void drawBorder();
		void drawBody();
};

//------------------------ Platforma z łamanymi, wolniejsza

/**
 * Brak dziedziczenia od Renderer, to ma
 * być klasa magazynująca!
 */

class IrregularPlatform: public Platform {
	protected:
		PlatformShape* shape;
		float scale;

	public:
		IrregularPlatform(float, float, usint, PlatformShape*, float = -1.f);

		virtual void drawObject(Window*);

		/**
		 * Kształt platformy!
		 */
		PlatformShape* getShape() const {
			return shape;
		}
		
		void setShape(PlatformShape*);

		/**
		 * Klonowanie obiektów!
		 */
		virtual Cloneable* getClone() const {
			return new IrregularPlatform(*this);
		}
		
		/**
		 * Zmiana rozmiaru tylko
		 * do szerokości!
		 */
		void fitToWidth(float);

		/**
		 * Skalowanie całego obiektu!
		 */
		void setScale(float);

		float getScale() const {
			return scale;
		}
		
		virtual ~IrregularPlatform() {
			/**
			 * Ten sam kształt dla kilku platform!
			 */
		}
};

//------------------------ Przeciwnicy i gracz

/*
 * Klasa magazynująca dane o obiektcie
 *  nie będzie renderowana!
 */
struct CharacterStatus: public Resource<usint> {
		int health;
		int shield_health;
		int score;

		bool shield;
		Vector<float> start_pos;

		CharacterStatus() :
						Resource<usint>(NULL),
						health(0),
						shield_health(0),
						score(0),
						shield(false),
						start_pos(0, 0) {
		}
		
		CharacterStatus(int _health, bool _shield, int _shield_health,
				int _score, float _x = 0, float _y = 0) :
						Resource<usint>(NULL),
						health(_health),
						shield_health(_shield_health),
						score(_score),
						shield(_shield),
						start_pos(_x, _y) {
		}
		
		virtual bool load(FILE*);
		virtual void unload() {
		}
		
		CharacterStatus& operator+=(CharacterStatus& right) {
			health += right.health;
			shield = right.shield;
			shield_health += right.shield_health;
			score += right.score;
			return *this;
		}
		
		CharacterStatus& operator-=(CharacterStatus& right) {
			USINT_SUB(health, right.health);
			USINT_SUB(shield_health, right.shield_health);
			USINT_SUB(score, right.score);
			//
			shield = right.shield;
			return *this;
		}
};

/**
 * Każde AI tworzone jest dynamicznie!
 * Nie przez AllocKiller bo zwolni!
 */
class Character;
class AI {
	public:
		Character* character;

		AI(Character* _character) :
						character(_character) {
		}

		/**
		 * Sterowanie automatem, botem,
		 * potrzebny zatem wskaźnik do
		 * silnika fizycznego!
		 */
		virtual void drive() = 0;

		/**
		 * Wykrywanie kolizji, reakcja!
		 */
		virtual void getCollision(pEngine*, usint, Body*) = 0;

		virtual ~AI() {
		}
};

//-------------------------

/**
 * Generowanie 'krwii' ;)
 */
void generateExplosion(pEngine*, const Rect<float>&, usint, const Color&, float,
		float, const Vector<float>&, float = -1, usint = Body::NONE);

//

/**
 * Klasa GRACZA
 */
class Character: public IrregularPlatform {
	public:
		enum Action {
			JUMPING = 1 << 0,
			CLIMBING = 1 << 1,
			BLOODING = 1 << 2
		};

		/**
		 * Punkt przywrócenia gracza po śmierci
		 */
		struct _Checkpoint {
				/**
				 * Jeśli mapa jest zbyt zmodyfikowana
				 * musi zostać wczytana 2 raz i gra
				 * rozpoczyna się od nowa!
				 */
				bool reload_map;

				// Ostatni status gracza
				CharacterStatus last_status;
		};

		/**
		 * Animowany tekst np. po zdobyciu
		 * punktu unoszący się do góry
		 */
		struct _Tooltip {
				_Timer life_timer;
				glText text;
				Vector<float> pos;

				_Tooltip(const char* _text, const Vector<float>& _pos,
						const Color& _col) :
								life_timer(strlen(_text) * 11),
								text(_col, _text, GLUT_BITMAP_HELVETICA_18, 18),
								pos(_pos) {
				}
		};

	protected:
		/**
		 * Akcja gracza - jego aktualny stan
		 */
		usint action;

		/**
		 * Gracz/wrogowie modyfikują ciągle
		 * swój status, dlatego musi być
		 * statyczny!
		 */
		CharacterStatus status;

		/**
		 * Długość wyświetlenia pojedynczej
		 * klatki zaczerwienienia gracza
		 */
		_Timer blood_anim_visible_time;

		/**
		 * Ilość klatek zaczerwienienia
		 */
		_Timer blood_anim_cycles;

		/**
		 * Lewitacja obiektu - przeznaczone
		 * dla punktu
		 */
		_Timer levitation_timer;
		Vector<float> start_pos;

		/**
		 * Zamiast stosu lepiej dać ostatni
		 */
		_Checkpoint last_checkpoint;

		/**
		 * Chmurki latające
		 */
		deque<_Tooltip> tooltips;

	public:
		/**
		 * Kolor platformy to kolor nicku,
		 * kolorem gracza jest info z
		 * PlatformShape
		 */
		Character(const string&, float, float, PlatformShape*, usint);

		virtual void drawObject(Window*);
		virtual void catchCollision(pEngine*, usint, Body*);

		/**
		 * Czy postać aktualnie jest czerwona
		 * od krwii
		 */
		bool isBlooding() const {
			return blood_anim_cycles.cycles_count % 2
					&& IS_SET(action, BLOODING);
		}

		bool isJumping() const {
			return IS_SET(action, JUMPING);
		}
		
		bool isDead() const {
			return status.health == DEATH;
		}
		
		usint getAction() const {
			return action;
		}

		inline void addTooltip(const char* _text, const Color& _col) {
			tooltips.push_back(_Tooltip(_text, Vector<float>(x, y), _col));
		}

		/**
		 * Przepisywanie informacji do
		 * checkpointa
		 */
		void addCheckpoint(bool);
		void recoverFromCheckpoint(pEngine*);

		bool isCheckpointAvailable() const {
			return !last_checkpoint.reload_map;
		}

		/**
		 * Akcje dotyczące poruszania się i
		 * zachowania gracza
		 */
		void die(pEngine*); // śmierć, rozprucie ;)
		void hitMe(pEngine*); // uderz mnie ;_;
				
		void move(float, float);
		void jump(float, bool);

		void dodge(usint); // taktyczny unik

		/**
		 * Odświeżanie gracza
		 */
		void updateMe();

		/**
		 * Klonowanie obiektów!
		 */
		virtual Cloneable* getClone() const {
			return new Character(*this);
		}
		
		virtual bool recover(Cloneable* _clone) {
			Character* obj = dynamic_cast<Character*>(_clone);
			if (!obj) {
				return false;
			}
			(*this) = *obj;
			return true;
		}
		
		void setStatus(const CharacterStatus& _status) {
			status = _status;
		}
		
		CharacterStatus* getStatus() {
			return &status;
		}
		
		virtual ~Character() {
		}
		
	private:
		/**
		 * Odświeżanie..
		 */
		void updateHitAnim();

		/**
		 * Rysowanie elementów opcjonalnych..
		 */
		void drawTooltips();
};

/**
 * Obiekt generujący skrypt
 */
class Trigger: public Body {
	private:
		Script* script;

	public:
		Trigger(Script* _script, float _x, float _y, float _w, float _h) :
						Body(_x, _y, _w, _h, 1.f, 1.f, Body::HIDDEN),
						script(_script) {
			type = Body::TRIGGER;
			state = Body::HIDDEN;
		}

		/**
		 * Generowanie zdarzenia!
		 */
		inline void generate() {
			if (destroyed) {
				return;
			}
			if (Interpreter::getIstance().interpret(script)) {
				destroyed = true;
			}
		}
		
		~Trigger() {
			if (script) {
				delete script;
			}
		}
};

/**
 * Lawa nie jest obiektem, nie podlega
 * fizyce ale jeśli gracz w nią wpadnie
 * to ginie
 */
class Lava: public Body {
	private:
		/**
		 * Płomień działa jak parallax
		 * są ich 2
		 */
		Vector<float> pos[2];
		PlatformShape* flame_shapes[2];

	public:
		Lava(float, float);

		virtual void drawObject(Window*);

	protected:
		// Odświeżanie
		void update();
};

/**
 * Wzorzec singleton!
 */
using oglWrapper::Shader;

class ResourceFactory {
	public:
		/**
		 * Kolory tekstur dla różnych leveli
		 * np. dla zimy będą chłodne
		 */
		enum TextureTemperature {
			ICY,
			NEUTRAL,
			HOT
		};

		/**
		 * Typy obiektów generowanych
		 * przez fabryke nie są tym
		 * samym co w silniku fizycznym
		 */
		enum Types {
			SCORE,
			HEALTH,
			GHOST,
			OBJECT,
			GUN, // broń
			SCRIPT_BOX, // skrypt
			SPIKES, // kolce
			LADDER, // drabina
			LIANE, // liana
			LAVA, // lawa
			/**
			 * Dynamiczne obiekty nie są wczytywane
			 * dlatego idą na koniec
			 */
			BULLET,
			/**
			 * Obiekty niewidoczne
			 */
			KILLZONE
		};

		/**
		 * Poziom trudności
		 */
		enum HardLevel {
			EASY,
			NORMAL,
			HARD
		};

		/**
		 * Statusy obiektów
		 */
		struct _FactoryStatus {
				// Info dla silnika
				usint character_type;
				usint state;

				// Informacje o postaci
				bool is_score;
				CharacterStatus character_status;
		};

		/**
		 * Tekstury obiektów w fabryce
		 */
		struct _TextureConfig {
				// Podstawowe info
				usint type;
				usint orientation;
				float rotation;
				float width;

				// Dla zasobu
				const char* file_name;
				const char* resource_label;

				/**
				 * Nie wszystkie tekstury muszą
				 * mieć zmienną temperaturę
				 */
				bool temperature_enabled;

				// Identyfikator
				usint resource_id;
		};

		static _TextureConfig factory_types[];
		static map<usint, _FactoryStatus> factory_status;

		void generateChracterStatus(usint);

		/**
		 * Wyszukiwanie typu obiektu
		 */
		static _TextureConfig* getFactoryType(usint _type, usint _orientation) {
			/**
			 * Różne typy są w różnych orientacjach
			 */
			for (usint i = 0; i < 16; ++i) {
				_TextureConfig* obj = &factory_types[i];
				//
				if (obj->type == _type && obj->orientation == _orientation) {
					return &factory_types[i];
				}
			}
			return NULL;
		}

	private:
		deque<Body*> created;
		deque<Trigger*> triggers;

		/**
		 * PODSTAWOWE ZASOBY!!!
		 */
		map<usint, PlatformShape*> textures;

		// Temperatura tekstur
		usint texture_temperature;

		// Fizyka
		pEngine* physics;

		ResourceFactory();

	public:
		/**
		 * Zmiana wystroju tekstur
		 */
		void changeTemperatureOfTextures(usint);

		/**
		 * Dodawanie tekstury
		 */
		void putTexture(usint, PlatformShape*);

		/**
		 * Deklaruje dynamicznie!
		 */
		Body* createObject(usint, float, float, float, float, PlatformShape*,
				char*, usint);

		/**
		 * Kasowanie obiektów mapy!
		 */
		void unloadObjects();

		/**
		 * Pobieranie tekstury!
		 */
		PlatformShape* getTexture(usint, usint);

		/**
		 * Temperatura
		 */
		usint getTextureTemperature() const {
			return texture_temperature;
		}

		/**
		 * Pobieranie instancji i inicjowanie!
		 */
		static ResourceFactory& getInstance(pEngine*);

	private:
		void loadMainTexturesPack();
		void loadMobsTexturesPack(const char*, bool);

		/**
		 *  Generowanie kolejnych id dla poszczególnych orientacji
		 */
		usint genTextureID(usint, usint) const;

		void addBody(Body*);
};

#endif /* OBJECTS_HPP_ */
