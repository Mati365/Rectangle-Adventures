/*
 * Resources.cpp
 *
 *  Created on: 13 lip 2013
 *      Author: mateusz
 */
#include "Resources.hpp"

#include "../../Engine/Sound/Sounds.hpp"

Package main_filesystem(FILESYSTEM_PACKAGE, FILESYSTEM_AUTHOR);
ResourceManager<usint> main_resource_manager;

/**
 * Sprawdzenie autentyczności archiwum!
 */
bool isFilesystemFake() {
	return (strcmp(main_filesystem.getHeader()->author, FILESYSTEM_AUTHOR) != 0);
}

////////////////////////// Dźwięki

using namespace Sound;

_Sound sounds[DIE_SOUND + 1];

/**
 * Wczytywanie paczki dźwięków!
 */
void loadSoundsPack() {
	sounds[JUMP_SOUND] = {wavPlayer::getInstance().loadSound("skok.wav"), MIX_MAX_VOLUME / 2};
	sounds[GUN_SHOT_SOUND] = {wavPlayer::getInstance().loadSound("laser.wav"), MIX_MAX_VOLUME / 8};
	sounds[MENU_CHOOSE_SOUND] = {wavPlayer::getInstance().loadSound("wybor_menu.wav"), MIX_MAX_VOLUME / 2};
	sounds[SCORE_SOUND] = {wavPlayer::getInstance().loadSound("punkt.wav"), MIX_MAX_VOLUME / 4};
	sounds[SPIKES_SOUND] = {wavPlayer::getInstance().loadSound("igly.wav"), MIX_MAX_VOLUME};
	sounds[DIE_SOUND] = {wavPlayer::getInstance().loadSound("smierc.wav"), MIX_MAX_VOLUME};
}

void unloadSoundsPack() {
	for (auto& obj : sounds) {
		wavPlayer::getInstance().closeChunk(obj.chunk);
	}
}

////////////////////////// Shadery

Shader* shaders[HIT_CHARACTER_SHADER + 1];

/**
 * Wczytywanie paczki shaderów!
 */
void loadShadersPack() {
	shaders[WINDOW_SHADOW_SHADER] = new Shader(
			NULL,
			main_filesystem.getExternalFileContent("shadow_frag_shader.txt"),
			NULL);
	
	shaders[HIT_CHARACTER_SHADER] = new Shader(
			NULL,
			main_filesystem.getExternalFileContent("hit_frag_shader.txt"),
			NULL);
	//
	logEvent(Logger::LOG_INFO, "Pomyślnie wczytano paczkę shaderów!");
}

void unloadShadersPack() {
	for (auto* shader : shaders) {
		delete shader;
	}
}
