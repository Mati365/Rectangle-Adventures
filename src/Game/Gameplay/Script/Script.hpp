/*
 * Script.hpp
 *
 *  Created on: 15 lip 2013
 *      Author: mateusz
 */

#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_
#include "../../Tools/Tools.hpp"

#define ARG_COUNT 8 // Maksymalna ilość argumentów!
#define SPACE_CHARACTER '%' // Znak spacji w skryptach
/** Deklaracja funkcji */
struct Func {
		usint id; // identyfikator
		char func_name[40]; // nazwa funkcji
		usint argc; // ilość argumentów
		char args[ARG_COUNT][255]; //argumenty
};

/** Typy skryptu! */
enum {
	COLLISION,
	JUMP
};

/** Typy particle */
enum {
	FIRE
};

/** Typy pogody */
enum {
	// 0 - spadajace kwadraty
	SHAKE = 1,
	FIREWORKS = 2, // fajerwerki
	SNOW = 3 // śnieg
};

/** Deklaracje!! */
enum {
	SCRIPT_TYPE,
	SELECT_PLATFORM,
	SET_STATE,
	SET_LAYER,
	SET_MOVING_DIR,
	ATTACH_PARTICLE,
	DISABLE_MOVING,
	DESTROY_OBJECT,
	ADD_WEATHER,
	SHOW_MESSAGE,
	SHOW_SPLASH,
	NEXT_LEVEL,
	ADD_CHECKPOINT,
	CREATE_OBJECT
};

extern Func funcs[];

/**
 * Interpreter!
 * Wzorzec singleton!
 */
class Script {
	public:
		size_t length;
		Func* commands;

		Script(size_t _length) :
						length(_length) {
			commands = new Func[length];
		}
		
		~Script() {
			if (length != 0 && commands) {
				delete[] commands;
			}
		}
};

/** Singleton, na później.. */
class Interpreter {
	private:
		Interpreter() {
		}
		
	public:
		static Interpreter& getIstance() {
			static Interpreter inter;
			//
			return inter;
		}
		/**
		 * Kompilacja z tekstu!
		 * Zwracanie dynamicznej wartości!
		 * Przyjmowanie również dynamiczne!
		 */
		Script* compile(char*);

		/** Interpretacja ze skryptu!*/
		bool interpret(Script*);
};
#endif /* SCRIPT_HPP_ */
