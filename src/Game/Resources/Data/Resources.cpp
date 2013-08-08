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

Mix_Chunk* sounds[JUMP_SOUND + 1];

/**
 * Wczytywanie paczki dźwięków!
 */
void loadSoundsPack() {
	sounds[JUMP_SOUND] = wavPlayer::getInstance().loadSound("skok.wav");
}

void unloadSoundsPack() {
	for (auto* obj : sounds) {
		wavPlayer::getInstance().closeChunk(obj);
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
