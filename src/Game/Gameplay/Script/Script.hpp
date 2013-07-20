/*
 * Script.hpp
 *
 *  Created on: 15 lip 2013
 *      Author: mateusz
 */

#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_
#include "../../Tools/Tools.hpp"

#define ARG_COUNT 5 // Maksymalna ilość argumentów!
#define SPACE_CHARACTER '%'

/**
 * Deklaracja funkcji
 */
struct Func {
		usint id; // identyfikator
		char func_name[40]; // nazwa funkcji
		usint argc; // ilość argumentów
		char args[ARG_COUNT][255]; //argumenty
};

/**
 * Deklaracje!!
 */
enum {
	SHOW_MESSAGE, SHOW_SPLASH, LOAD_MAP, CREATE_OBJECT
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

/**
 * Singleton, na później..
 */
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
		/**
		 * Interpretacja z tekstu!
		 */
		bool interpret(Script*);
};
#endif /* SCRIPT_HPP_ */
