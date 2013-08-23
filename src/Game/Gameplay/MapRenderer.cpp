/*
 * MapRenderer.cpp
 *
 *  Created on: 16-03-2013
 *      Author: mati
 */
#include <GL/gl.h>
#include <GL/glut.h>

#include "Gameplay.hpp"
#include "Particle/Particle.hpp"

using namespace Gameplay;

/**
 * Główny renderer mapy
 */
MapRenderer::MapRenderer(Body* _hero, MapINFO* _map) :
				ParalaxRenderer(_hero, 0.95f, true),
				msg(45, Color(0, 128, 255), Color(255, 255, 255), this),
				hero(nullptr),
				hud_enabled(true),
				main_shader_id(WINDOW_SHADOW_SHADER),
				shadow_radius(DEFAULT_SHADOW_RADIUS),
				buffer_map(nullptr),
				buffer_swap_required(false) {
	setHero(dynamic_cast<Character*>(_hero));
	setMap(_map);

	resetColorSaturation();
}

/**
 * Dodawanie tła gry!
 */
ParalaxRenderer* MapRenderer::addToParalax(MapINFO* _paralax, float _ratio,
		Body* _body) {
	if (map->physics) {
		ParalaxRenderer* renderer = new ParalaxRenderer(
				_body,
				_ratio,
				true,
				_paralax);
		paralax_background.push_front(renderer);
		return renderer;
	}
	return nullptr;
}

/**
 * Eventy otrzymywane z okna
 */
void MapRenderer::catchEvent(const Event& _event) {
	if (!hero) {
		return;
	}
	if (!IS_SET(msg.getScreen(), MessageRenderer::HUD_SCREEN)) {
		msg.catchEvent(_event);
		return;
	}
	switch (_event.type) {
		/**
		 *
		 */
		case Event::KEY_PRESSED:
			if (hero) {
				if (_event.key == 'w') {
					hero->jump(5.f, false);
				} else if (_event.key == 'a') {
					hero->move(-2.f, 0.f);
				} else if (_event.key == 'd') {
					hero->move(2.f, 0.f);
				}
			}
			break;

			/**
			 *
			 */
		case Event::MOUSE_CLICKED:
			break;
	}
}

/**
 * Dodawanie pogody
 * AllocKiller, brak wycieku!
 */
void MapRenderer::addWeather(usint _type) {
	switch (_type) {
		/**
		 * Śnieg
		 */
		case SNOWING: {
			SnowEmitter* snow = new SnowEmitter(
					Rect<float>(0, 20, WINDOW_WIDTH, 0));
			snow->setFocus(&cam.pos);
			//
			addStaticObject(snow);
		}
			break;

			/**
			 * Erupcja wulkanu
			 */
		case SHAKE:
			shake();
			break;

			/**
			 * Sztuczne ognie
			 */
		case FIREWORKS: {
			FireworksEmitter* fireworks = new FireworksEmitter(
					Rect<float>(
							cam.pos.x,
							cam.pos.y,
							WINDOW_WIDTH,
							WINDOW_HEIGHT),
					50,
					map->physics);
			//
			addStaticObject(fireworks);
		}
			break;

			/**
			 *
			 */
		default:
			break;
	}
}

/**
 * Podmienienie mapy buforowej ze zwykłą
 */
void MapRenderer::swapBufferMap() {
	buffer_swap_required = true;
}

void MapRenderer::setMap(MapINFO* _map) {
	for (auto& obj : static_objects) {
		safe_delete<Body>(obj);
	}
	static_objects.clear();

	shake_timer.reset();
	shake_timer.active = false;

	/**
	 * Ustawienia
	 */
	addWeather(_map->map_weather);
	ResourceFactory::getInstance(_map->physics).changeTemperatureOfTextures(
			_map->map_temperature);

	safe_delete<MapINFO>(map);
	map = _map;

	/** Reset gracza */
	resetHero();
}

/**
 * Resetowanie gracza
 */
void MapRenderer::resetHero() {
	if (!hero || !map) {
		return;
	}
	if (hero->getShape()
			&& strcmp(hero->getShape()->getLabel(), "cranium") == -1) {
		main_resource_manager.deleteResource(hero->getShape()->getResourceID());
	}
	hero->setShape(map->hero_shape);
	hero->fitToWidth(map->hero_bounds.w);

	hero->x = map->hero_bounds.x;
	hero->y = map->hero_bounds.y;

	hero->getStatus()->health = MAX_LIVES;
	hero->getStatus()->score = 0;

	hero->with_observer = true;

	map->physics->insert(hero);
}

/**
 * Ustawienie focusa kamery i bohatera
 */
void MapRenderer::setHero(Character* _hero) {
	// Ustawienie nowego gracza
	hero = _hero;
	cam.focus = hero;
}

/**
 * Pokazywanie game over
 */
void MapRenderer::showGameOver() {
	if (hud_enabled && msg.getScreen() != MessageRenderer::DEATH_SCREEN) {
		/**
		 * A co jeśli jest może checkpoint?
		 */
		if (hero->isCheckpointAvailable()) {
			hero->recoverFromCheckpoint(map);
			//
			resetColorSaturation();
			shadow_radius = DEFAULT_SHADOW_RADIUS;
		} else {
			/**
			 * Nie ma checkpointu ;(
			 */
			msg.setScreen(MessageRenderer::DEATH_SCREEN);
		}
	}
}

/**
 * Rysowanie
 */
void MapRenderer::drawObject(Window* _window) {
	shaders[main_shader_id]->begin();
	shaders[main_shader_id]->setUniform2f(
			"center",
			hero->x + hero->w / 2 - cam.pos.x,
			hero->y + hero->h / 2 - cam.pos.y);
	shaders[main_shader_id]->setUniform3f(
			"active_colors",
			col_saturation[0],
			col_saturation[1],
			col_saturation[2]);
	shaders[main_shader_id]->setUniform1f("radius", shadow_radius);

	/**
	 * Główny rendering mapy - najpierw paralaksa
	 */
	for (usint i = 0; i < paralax_background.size(); ++i) {
		paralax_background[i]->drawObject(_window);
	}
	ParalaxRenderer::drawObject(_window);

	shaders[main_shader_id]->end();

	/**
	 * Sprawdzenie stanu gracza oraz dopasowanie do niego
	 * shaderu
	 * Ten sam efekt dla konca gry jak i tez wczytywania
	 */
	if (buffer_map && msg.getActiveScreen() == MessageRenderer::DEATH_SCREEN) {
		msg.setScreen(MessageRenderer::HUD_SCREEN);
		resetColorSaturation();
	}
	if (hero->isDead() || (buffer_map && buffer_swap_required)) {
		if (col_saturation[0] == 0.f) {
			/**
			 * WINDOW_DEATH_SHADER
			 */
			shadow_radius += 0.7f;
			if (shadow_radius >= DEFAULT_SHADOW_RADIUS) {
				shadow_radius = DEFAULT_SHADOW_RADIUS;
				if (!hero->isDead()) {
					buffer_map = nullptr;
				}
			}
			if (hero->isDead()) {
				col_saturation[1] = (float) shadow_radius
						/ (float) DEFAULT_SHADOW_RADIUS;
			}
		} else {
			/**
			 * WINDOW_SHADOW_SHADER
			 */
			if (!IS_SET(hero->getAction(), Character::BLOODING) || buffer_map) {
				shadow_radius -= 0.7f;
				if (shadow_radius <= 50) {
					if (buffer_map) {
						swapBufferMap();
					} else {
						showGameOver();
					}
					col_saturation[0] = 0.f;
				}
			}
		}
	} else {
		resetColorSaturation();
	}
	/**
	 * Elementy HUDu
	 */
	if (hud_enabled) {
		msg.drawObject(_window);
	}
	/**
	 * Podmiana buforu - bezpieczniejsza
	 */
	if (buffer_swap_required) {
		setMap(buffer_map);

		buffer_map = nullptr;
		buffer_swap_required = false;

		shadow_radius = DEFAULT_SHADOW_RADIUS;
	}
}

MapRenderer::~MapRenderer() {
	for (auto& paralax : paralax_background) {
		if (paralax) {
			safe_delete<ParalaxRenderer>(paralax);
		}
	}
	safe_delete<Character>(hero);
}

