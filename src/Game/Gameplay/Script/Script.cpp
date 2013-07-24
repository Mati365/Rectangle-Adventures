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

Func funcs[] = {
					{
						SELECT_PLATFORM,
						"SELECT_PLATFORM",
						1 },
					{
						SET_STATE,
						"SET_STATE",
						1 },
					{
						SET_MOVING_DIR,
						"SET_MOVING_DIR",
						5 },
					{
						SHOW_MESSAGE,
						"SHOW_MESSAGE",
						2 },
					{
						SHOW_SPLASH,
						"SHOW_SPLASH",
						1 },
					{
						LOAD_MAP,
						"LOAD_MAP",
						1 },
					{
						CREATE_OBJECT,
						"CREATE_OBJECT",
						4 } };

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
	//
	while (tok != NULL) {
		for (usint i = 0; i < 8; ++i) {
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
			arg_ac = 0;
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
	Platform* selected = NULL;
	//
	for (usint i = 0; i < script->length; ++i) {
		Func func = script->commands[i];
		/**
		 * Sprawdzenie zaznaczenia!
		 */
		if (!selected && (func.id == SET_STATE || func.id == SET_MOVING_DIR)) {
			logEvent(Logger::LOG_WARNING, "Brak zaznaczenia!");
			continue;
		}
		switch (func.id) {
			/**
			 * Oznaczenie platform, wyszukiwanie platformy
			 * z listy platform!
			 */
			case SELECT_PLATFORM: {
				deque<Body*>* list = map->getPhysics()->getList();
				int id = Convert::stringTo<int>(func.args[0]);
				for (auto iter = list->begin(); iter != list->end(); ++iter) {
					/**
					 * Sprawdzenie wymiarów platformy, jeśli równe to znaleziono!
					 */
					if ((*iter)->script_id == id) {
						/**
						 *   Rzutowanie na platforme, jeśli to nie platforma to zwróci
						 *   nulla
						 */
						selected = dynamic_cast<Platform*>(*iter);
						break;
					}
				}
			}
				break;

				/**
				 * Uswiawienie stanu platformy!
				 */
			case SET_STATE:
				selected->setState(Convert::stringTo<usint>(func.args[0]));
				break;

				/**
				 * Ustawienie kierunku ruchu!
				 */
			case SET_MOVING_DIR:
				selected->setMovingDir(
						Vector<float>(Convert::stringTo<float>(func.args[0]),
										Convert::stringTo<float>(func.args[1])),
						Vector<float>(Convert::stringTo<float>(func.args[2]),
										Convert::stringTo<float>(func.args[3])),
						Convert::stringTo<bool>(func.args[4]));
				break;

				/**
				 * Pokazywanie wiadomości intro!
				 */
			case SHOW_MESSAGE:
				map->getMessageRenderer()->addMessage(
						MessageRenderer::Message(func.args[0], func.args[1],
						NULL));
				break;

				/**
				 * Wyświetlanie splash!
				 */
			case SHOW_SPLASH:
				active_screen = splash;
				//
				splash->unload();
				splash->pushTitle(func.args[0], strlen(func.args[0]) * 10);
				splash->endTo(game);
				break;

				/**
				 * Tworzenie obiektu!
				 */
			case CREATE_OBJECT:
				ObjectFactory::getIstance(map->getPhysics()).createObject(
						(ObjectFactory::Types) Convert::stringTo<int>(
								func.args[0]),
						Convert::stringTo<float>(func.args[1]),
						Convert::stringTo<float>(func.args[2]), 0, 0, NULL,
						NULL)->script_id = Convert::stringTo<usint>(
						func.args[3]);
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
