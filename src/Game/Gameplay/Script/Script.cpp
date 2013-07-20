/*
 * Script.cpp
 *
 *  Created on: 15 lip 2013
 *      Author: mateusz
 */
#include <string.h>

#include "Script.hpp"
#include "../Screens/Screens.hpp"

#include "../../Tools/Logger.hpp"
#include "../../Tools/Converter.hpp"

using namespace GameScreen;

//---------------------

Func funcs[] = { { SHOW_MESSAGE, "SHOW_MESSAGE", 2 }, {
		SHOW_SPLASH, "SHOW_SPLASH", 1 }, { LOAD_MAP, "LOAD_MAP", 1 }, {
		CREATE_OBJECT, "CREATE_OBJECT", 3 } };

//---------------------

/**
 * Kompilacja!
 */
Script* Interpreter::compile(char* str) {
	size_t len = strlen(str);
	if (len == 0) {
		return NULL;
	}
	deque<Func> array;
	usint arg_ac = 0;
	char* tok = strtok(str, " ");
	while (tok != NULL) {
		for (usint i = 0; i < 4; ++i) {
			if (strcmp(tok, funcs[i].func_name) == 0) {
				array.push_back(funcs[i]);
				arg_ac = 0;
				break;
			}
		}
		if (!array.empty() && arg_ac > 0 && arg_ac - 1 < array.back().argc) {
			char* ptr = array.back().args[arg_ac - 1];
			strcpy(ptr, tok);
			for (usint i = 0; i < strlen(ptr); ++i) {
				if (ptr[i] == SPACE_CHARACTER) {
					ptr[i] = ' ';
				}
			}
		} else if (arg_ac - 1 >= array.back().argc) {
			logEvent(Logger::LOG_ERROR,
					"Nieprawidłowa ilość argumentów funkcji!");
			return NULL;
		}
		arg_ac++;
		tok = strtok(NULL, " ");
	}
	if (array.empty()) {
		return NULL;
	}
	Script* script = new Script(array.size());
	for (usint i = 0; i < script->length; ++i) {
		script->commands[i] = array[i];
	}
	return script;
}

/**
 * Interpretacja!
 */
bool Interpreter::interpret(Script* script) {
	if (!script || !game) {
		return false;
	}
	MapRenderer* map = game->getMapRenderer();

	for (usint i = 0; i < script->length; ++i) {
		Func func = script->commands[i];
		switch (func.id) {
			case SHOW_MESSAGE:
				map->getMessageRenderer()->addMessage(
						MessageRenderer::Message(func.args[0], func.args[1],
						NULL));
				break;
				/**
				 *
				 */
			case SHOW_SPLASH:
				active_screen = splash;
				//
				splash->pushTitle(func.args[0],
						strlen(func.args[0]) * 10);
				break;
				/**
				 *
				 */
			case CREATE_OBJECT:
				ObjectFactory::getIstance(map->getPhysics()).createObject(
						(ObjectFactory::Types) Convert::stringTo<int>(
								func.args[0]),
						Convert::stringTo<float>(func.args[1]),
						Convert::stringTo<float>(func.args[2]), 0, 0, NULL);
				break;
				/**
				 *
				 */
			default:
				return false;
		}
	}
	return true;
}
