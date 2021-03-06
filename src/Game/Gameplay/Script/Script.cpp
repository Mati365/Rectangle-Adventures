/*
 * Script.cpp
 *
 *  Created on: 15 lip 2013
 *      Author: mateusz
 */
#include <string.h>

#include "Script.hpp"

#include "../Particle/Particle.hpp"

#include "../Screens/Screens.hpp"
#include "../LevelManager.hpp"

#include "../../Tools/Logger.hpp"
#include "../../Tools/Converter.hpp"

using namespace GameScreen;

//---------------------

Func funcs[] = {
					{ SCRIPT_TYPE, "SCRIPT_TYPE", 1 },
					{ SELECT_PLATFORM, "SELECT_PLATFORM", 1 },
					{ SET_STATE, "SET_STATE", 1 },
					{ SET_MOVING_DIR, "SET_MOVING_DIR", 5 },
					{ SET_LAYER, "SET_LAYER", 1 },
					{ ATTACH_PARTICLE, "ATTACH_PARTICLE", 3 },
					{ DISABLE_MOVING, "DISABLE_MOVING", 0 },
					{ DESTROY_OBJECT, "DESTROY_OBJECT", 0 },
					{ ADD_WEATHER, "ADD_WEATHER", 2 },
					{ SHOW_MESSAGE, "SHOW_MESSAGE", 2 },
					{ SHOW_SPLASH, "SHOW_SPLASH", 1 },
					{ NEXT_LEVEL, "NEXT_LEVEL", 0 },
					{ ADD_CHECKPOINT, "ADD_CHECKPOINT", 1 },
					{ LOOK_AT, "LOOK_AT", 0 },
					{ CREATE_OBJECT, "CREATE_OBJECT", 5 } };

//---------------------

/** Kompilacja! */
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
		for (usint i = 0; i < CREATE_OBJECT + 1; ++i) {
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
		} else if (!array.empty() && arg_ac - 1 >= array.back().argc) {
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

/** Interpretacja! */
bool Interpreter::interpret(Script* script) {
	if (!script || !game) {
		return false;
	}
	MapRenderer* map = game->getMapRenderer();
	Platform* selected = NULL;
	//
	for (usint i = 0; i < script->length; ++i) {
		Func func = script->commands[i];
		/** Sprawdzenie zaznaczenia! */
		if (!selected
				&& (func.id == SET_STATE || func.id == SET_MOVING_DIR
						|| func.id == DISABLE_MOVING
						|| func.id == ATTACH_PARTICLE)) {
			logEvent(Logger::LOG_WARNING, "Brak zaznaczenia!");
			continue;
		}
		switch (func.id) {
			/**
			 * Nastepny poziom!
			 */
			case NEXT_LEVEL:
				LevelManager::getInstance().loadNextMap();
				break;
				
				/**
				 * Typ skryptu!
				 */
			case SCRIPT_TYPE: {
				int type = Convert::stringTo<int>(func.args[0]);
				switch (type) {
					case JUMP:
						if (!map->getHero()->isJumping()
								|| map->getHero()->velocity.y > 0) {
							return false;
						}
						break;
						/**
						 *
						 */
					default:
						break;
				}
			}
				break;
				
				/**
				 * Warstwa
				 */
			case SET_LAYER:
				selected->layer = Convert::stringTo<usint>(func.args[0]);
				break;
				
				/**
				 * Oznaczenie platform, wyszukiwanie platformy
				 * z listy platform!
				 */
			case SELECT_PLATFORM: {
				deque<Body*>* list = map->getPhysics()->getList();
				int id = Convert::stringTo<int>(func.args[0]);
				for (auto iter = list->begin(); iter != list->end(); ++iter) {
					/** Sprawdzenie wymiarow platformy, jesli rowne to znaleziono! */
					if ((*iter)->script_id == id) {
						/**
						 *  Rzutowanie na platforme, jesli to nie platforma to zwroci
						 *  nulla
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
				 * Wylaczenie ruchu!
				 */
			case DISABLE_MOVING:
				selected->disableMoving();
				break;
				
				/**
				 * Zniszczenie obiektu!
				 */
			case DESTROY_OBJECT:
				selected->destroyed = true;
				break;
				
				/**
				 * Dodanie pogody!
				 */
			case ADD_WEATHER: {
				switch (Convert::stringTo<usint>(func.args[0])) {
					/**
					 *
					 */
					case SHAKE:
						map->addWeather(MapRenderer::SHAKE);
						break;
						
						/**
						 *
						 */
					case FIREWORKS:
						map->addWeather(MapRenderer::FIREWORKS);
						break;
						
						/**
						 *
						 */
					case SNOW:
						map->addWeather(MapRenderer::SNOWING);
						break;
						
						/**
						 *
						 */
					default:
						break;
				}
				ResourceFactory::getInstance(NULL).changeTemperatureOfTextures(
						Convert::stringTo<usint>(func.args[1]));
			}
				break;
				
				/**
				 * Dodawanie checkpointu!
				 */
			case ADD_CHECKPOINT:
				map->getHero()->addCheckpoint(
						Convert::stringTo<bool>(func.args[0]));
				break;
				
				/**
				 * Ustawienie kierunku ruchu!
				 */
			case SET_MOVING_DIR:
				selected->setMovingDir(
						Vector<float>(
								Convert::stringTo<float>(func.args[0]),
								Convert::stringTo<float>(func.args[1])),
						Vector<float>(
								Convert::stringTo<float>(func.args[2]),
								Convert::stringTo<float>(func.args[3])),
						Convert::stringTo<bool>(func.args[4]));
				break;
				
				/**
				 * 'Doczepienie' particle do obiektu!
				 */
			case ATTACH_PARTICLE: {
				usint particle_type = Convert::stringTo<usint>(func.args[0]);
				ParticleEmitter* emitter = NULL;
				//
				switch (particle_type) {
					case FIRE:
						emitter = new FireEmitter(
								Rect<float>(0, 0, 36, 64),
								25);
						break;
						/**
						 *
						 */
					default:
						logEvent(Logger::LOG_WARNING, "Nieznany typ particle!");
						break;
				}
				if (emitter) {
					emitter->setFocus(selected);
					emitter->setPosInBody(
							Vector<float>(
									Convert::stringTo<float>(func.args[1]),
									Convert::stringTo<float>(func.args[2])));
					game->getMapRenderer()->addStaticObject(emitter);
				}
			}
				break;
				
				/**
				 * Pokazywanie wiadomosci intro!
				 */
			case SHOW_MESSAGE:
				map->getMessageRenderer()->addMessage(
						MessageRenderer::Message(
								func.args[0],
								func.args[1],
								NULL));
				break;
				
				/**
				 * Wyswietlenie splash!
				 */
			case SHOW_SPLASH:
				active_screen = splash;
				//
				splash->unload();
				splash->pushTitle(func.args[0], strlen(func.args[0]) * 10);
				splash->endTo(game);
				break;
				
				/**
				 * Scrollowanie kamery do celu!
				 */
			case LOOK_AT:
				Camera::getFor().lookAt(selected);
				break;
				
				/**
				 * Tworzenie obiektu!
				 */
			case CREATE_OBJECT:
				ResourceFactory::getInstance(map->getPhysics()).createObject(
						(ResourceFactory::Types) Convert::stringTo<usint>(
								func.args[0]),
						Convert::stringTo<float>(func.args[1]),
						Convert::stringTo<float>(func.args[2]),
						0,
						0,
						NULL,
						NULL,
						Convert::stringTo<float>(func.args[4]))->script_id =
						Convert::stringTo<usint>(func.args[3]);
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
