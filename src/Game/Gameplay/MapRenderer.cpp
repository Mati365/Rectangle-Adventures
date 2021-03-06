/*
 * MapRenderer.cpp
 *
 *  Created on: 16-03-2013
 *      Author: mati
 */
#include "Gameplay.hpp"

#include "Particle/Particle.hpp"

using namespace Gameplay;

/** Konstruktor */
MapRenderer::MapRenderer(Body* _hero, MapINFO* _map) :
				ParalaxRenderer(_hero, DEFAULT_CAM_RATIO),
				msg(45, Color(0, 128, 255), Color(255, 255, 255), this),
				hero(nullptr),
				hud_enabled(true),
				main_shader_id(WINDOW_SHADOW_SHADER),
				shadow_radius(DEFAULT_SHADOW_RADIUS),
				buffer_map(nullptr),
				buffer_swap_required(false) {
	setHero(dynamic_cast<Character*>(_hero));
	setMap(_map);
	setConfig(RendererConfig::DRAW_QUAD);
	
	resetColorSaturation();
}

/** Dodawanie parakalksy */
ParalaxRenderer* MapRenderer::addToParalax(MapINFO* _paralax, float _ratio,
		Body* _body, usint _config) {
	if (map->physics) {
		ParalaxRenderer* renderer = new ParalaxRenderer(
				_body,
				_ratio,
				_paralax);
		renderer->setConfig(_config);
		
		paralax_background.push_front(renderer);
		//
		return renderer;
	}
	return nullptr;
}

/** Eventy otrzymywane z okna */
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
					hero->jump(7.f, false);
				} else if (_event.key == 'a') {
					hero->move(-1.3f, 0.f);
				} else if (_event.key == 'd') {
					hero->move(1.3f, 0.f);
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
		 * Snieg
		 */
		case SNOWING: {
			SnowEmitter* snow = new SnowEmitter(
					Rect<float>(0, 20, screen_bounds.x, 0));
			snow->setFocus(Camera::getFor().getPos());
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
							Camera::getFor().getPos()->x,
							Camera::getFor().getPos()->y,
							screen_bounds.x,
							screen_bounds.y),
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

/** Podmiana bufora mapy */
void MapRenderer::swapBufferMap() {
	buffer_swap_required = true;
}

void MapRenderer::setMap(MapINFO* _map) {
	static_objects.clear();
	
	shake_timer.reset();
	shake_timer.active = false;
	
	/** Wczytywanie pogody */
	addWeather(_map->map_weather);
	ResourceFactory::getInstance(_map->physics).changeTemperatureOfTextures(
			_map->map_temperature);
	
	safe_delete<MapINFO>(map);
	map = _map;
	
	/** Reset gracza */
	resetHero();
}

/** Reset gracza */
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
	
	map->physics->remove(hero); // dla pewnosci
	map->physics->insert(hero);
}

/** Ustawienie bohatera i ustawienie focusa kamery */
void MapRenderer::setHero(Character* _hero) {
	// Ustawienie nowego gracza
	hero = _hero;
	Camera::getFor(hero);
}

/** Pokazywanie game over */
void MapRenderer::showGameOver() {
	if (hud_enabled && msg.getScreen() != MessageRenderer::DEATH_SCREEN) {
		/** Reload do checkpointa */
		if (hero->isCheckpointAvailable()) {
			hero->recoverFromCheckpoint(map);
			//
			resetColorSaturation();
			shadow_radius = DEFAULT_SHADOW_RADIUS;
		} else {
			/** Nie ma checkpointu ;( */
			msg.setScreen(MessageRenderer::DEATH_SCREEN);
		}
	}
}

/** Obliczanie wspolczynnika przesuniecia */
void MapRenderer::calcCameraRatio() {
	if (!hero) {
		return;
	}
	
	/** Aby gracz nie wychodzil za ekran! */
	Vector<float> hero_screen_pos = Camera::getFor().getFocusScreenPos();
	
	/** Odleglosc max. ruchow gracza na ekranie */
	float go_distance = .75f;
	
	if (hero_screen_pos.x > screen_bounds.x * go_distance
			|| hero_screen_pos.x < screen_bounds.x * (1 - go_distance)
			|| hero_screen_pos.y > screen_bounds.y * go_distance
			|| hero_screen_pos.y < screen_bounds.y * (1 - go_distance)) {
		ratio = 1.f + (1.f - DEFAULT_CAM_RATIO); // z powrotem na srodek ekranu
	} else {
		ratio = DEFAULT_CAM_RATIO;
	}
}

/** Rysowanie */
void MapRenderer::drawObject(Window* _window) {
	if (!hero) {
		return;
	}
	/** Obliczenie wspolczynnika kamery */
	calcCameraRatio();
	
	/** Konfiguracja shadera */
	if (window_config.flag[WindowConfig::WITH_SHADERS]) {
		Vector<float> focus_pos = Camera::getFor().getFocusScreenPos();
		
		shaders[main_shader_id]->begin();
		shaders[main_shader_id]->setUniform2f(
				"center",
				focus_pos.x,
				focus_pos.y);
		shaders[main_shader_id]->setUniform3f(
				"active_colors",
				col_saturation[0],
				col_saturation[1],
				col_saturation[2]);
		shaders[main_shader_id]->setUniform1f("radius", shadow_radius);
	}
	
	/** Tlo */
	oglWrapper::drawFillRect(
			0,
			0,
			screen_bounds.x,
			screen_bounds.y,
			Color(3, 3, 3));
	
	/** Glowny rendering mapy - najpierw paralaksa  */
	for (usint i = 0; i < paralax_background.size(); ++i) {
		paralax_background[i]->drawObject(_window);
	}
	ParalaxRenderer::drawObject(_window);
	if (window_config.flag[WindowConfig::WITH_SHADERS]) {
		shaders[main_shader_id]->end();
	}
	
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
			shadow_radius += 3.f;
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
				shadow_radius -= 3.f;
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
	
	/** Elementy HUDu */
	if (hud_enabled) {
		msg.drawObject(_window);
	}
	
	/** Podmiana buforu - bezpieczniejsza */
	if (buffer_swap_required) {
		setMap(buffer_map);
		
		buffer_map = nullptr;
		buffer_swap_required = false;
		
		shadow_radius = DEFAULT_SHADOW_RADIUS;
	}
}

MapRenderer::~MapRenderer() {
	for (auto& paralax : paralax_background) {
		safe_delete<ParalaxRenderer>(paralax);
	}
}

