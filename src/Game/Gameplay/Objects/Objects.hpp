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

#define MAX_LIVES 3
#define MAX_SCORE 1000
#define DEATH -1

/**
 * Platforma, po której porusza się gracz
 */
class Platform: public Body, public Cloneable {
	public:
		enum Type {
			DIAGONAL,
			SIMPLE,
			FILLED,
			NONE
		};

	protected:
		Color col;
		/**
		 * Dla ruchomych elementów..
		 */
		Vector<float> distance;
		Vector<float> max_distance;
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
		IrregularPlatform(float, float, usint, PlatformShape*);

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
		void setScale(float _scale) {
			scale = _scale;
			h *= scale;
			w *= scale;
		}
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

		CharacterStatus(const char* _label) :
						Resource<usint>(_label),
						health(0),
						shield_health(MAX_LIVES),
						score(0),
						shield(true) {
		}
		
		CharacterStatus(const char* _label, usint _health, bool _shield,
				usint _shield_health, usint _score, float _x = 0, float _y = 0) :
						Resource<usint>(_label),
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
void generateExplosion(pEngine*, Body*, usint, const Color&, float, float);

//

class Character: public IrregularPlatform {
	public:
		enum Action {
			JUMPING = 1 << 0,
			CLIMBING = 1 << 1,
			BLOODING = 1 << 2
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
		AI* ai;

		/**
		 * Długość wyświetlenia pojedynczej
		 * klatki zaczerwienienia gracza
		 */
		_Timer blood_anim_visible_time;

		/**
		 * Ilość klatek zaczerwienienia
		 */
		_Timer blood_anim_cycles;

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
		bool isDrawingBlood() const {
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

		/**
		 * Akcje dotyczące poruszania się i
		 * zachowania gracza
		 */
		void die(pEngine*, usint); // śmierć, rozprucie ;)
		void hitMe(pEngine*); // uderz mnie ;_;
				
		void move(float, float);
		void jump(float, bool);

		void setAI(AI* _ai) {
			ai = _ai;
		}
		
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
			if (ai) {
				delete ai;
			}
		}
		
	private:
		void updateHitAnim();
};

/**
 * AI Ślimaka - poruszanie się w lewo i prawo
 * w poziomie
 */
class SnailAI: public AI {
	protected:
		float speed;

	public:
		SnailAI(Character*, float);

		/**
		 * Metody AI!
		 */
		virtual void drive();
		virtual void getCollision(pEngine*, usint, Body*);
};

/**
 *
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
			LAVA, // lawa
			/**
			 * Dynamiczne obiekty nie są wczytywane
			 * dlatego idą na koniec
			 */
			BULLET
		};

		/**
		 * Typy obiektów w fabryce
		 */
		struct FactoryType {
				usint type;
				usint orientation;
				float rotation;
				float width;
				// Dla zasobu
				const char* file_name;
				const char* resource_label;
		};

		static FactoryType factory_types[];

		/**
		 * Wyszukiwanie typu obiektu
		 */
		static FactoryType* getFactoryType(usint _type, usint _orientation) {
			for (usint i = 0; i < 12; ++i) {
				FactoryType* obj = &factory_types[i];
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

		/**
		 * STATUSY!!!!
		 */
		CharacterStatus health_status, score_status, ghost_enemy_status;
		pEngine* physics;

		ResourceFactory();

	public:
		/**
		 *
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
		 * Pobieranie instancji i inicjowanie!
		 */
		static ResourceFactory& getIstance(pEngine*);

	private:
		void loadTexturesPack();
		/**
		 *  Generowanie kolejnych id dla poszczególnych orientacji
		 */
		usint genTextureID(usint, usint) const;

		void addBody(Body*);
};

#endif /* OBJECTS_HPP_ */
