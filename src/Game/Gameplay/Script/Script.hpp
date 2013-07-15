/*
 * Script.hpp
 *
 *  Created on: 15 lip 2013
 *      Author: mateusz
 */

#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_
#include "../../Tools/Tools.hpp"

/**
 * Deklaracja funkcji
 */
struct Func {
		usint id; // identyfikator
		const char* func_name; // nazwa funkcji
		usint argc; // ilość argumentów
};

/**
 * Deklaracje!!
 */
enum {
	SET_INTRO_TEXT,
	LOAD_MAP,
	CREATE_OBJECT
};

Func funcs[] = {
		{ SET_INTRO_TEXT, "SET_INTRO_TEXT", 2 },
		{ LOAD_MAP, "LOAD_MAP", 2 },
		{ CREATE_OBJECT, "CREATE_OBJECT", 3 },
};

#endif /* SCRIPT_HPP_ */
