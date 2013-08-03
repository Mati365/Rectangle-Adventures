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

#include "../../Tools/Tools.hpp"

#include "../../Resources/Data/ResourceManager.hpp"
#include "../../Resources/Data/Resources.hpp"

#include "../Script/Script.hpp"

using namespace Engine;
using namespace Physics;

#define MAX_LIVES 3
#define MAX_SCORE 500
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
	protected:
		bool jumping;
		/**
		 * Gracz/wrogowie modyfikują ciągle
		 * swój status, dlatego musi być
		 * statyczny!
		 */
		CharacterStatus status;
		AI* ai;
		/**
		 * Animacja: mruganie czerwień/biel
		 * postaci
		 */
		Color source_color;

		usint actual_anim_time;
		usint anim_time;

		usint actual_cycles;
		usint anim_cycles;
		bool hit;

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
		 * Uderzenie, postać staje się naprzemian
		 * normalna
		 */
		bool isJumping() const {
			return jumping;
		}
		
		bool isDead() const {
			return status.health == DEATH;
		}
		
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
 * Wzorzec singleton!
 */
using oglWrapper::Shader;

class ResourceFactory {
	public:
		/**
		 * Typy obiektów!
		 */
		enum Types {
			SCORE,
			HEALTH,
			GHOST,
			OBJECT,
			GUN,
			GREEN_GUN,
			SCRIPT_BOX,
			/**
			 * Kolce
			 */
			SPIKES_UP,
			SPIKES_DOWN,
			SPIKES_LEFT,
			SPIKES_RIGHT
		};

		/**
		 * Typy shaderów
		 */
		enum Shaders {
			HIT_SHADER,
			WINDOW_SHADOW_SHADER
		};

	private:
		deque<Platform*> created;
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
				char*);

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
};

#endif /* OBJECTS_HPP_ */
