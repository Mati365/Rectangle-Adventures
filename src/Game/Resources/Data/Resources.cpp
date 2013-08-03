/*
 * Resources.cpp
 *
 *  Created on: 13 lip 2013
 *      Author: mateusz
 */
#include "Resources.hpp"

Package main_filesystem(FILESYSTEM_PACKAGE, FILESYSTEM_AUTHOR);
ResourceManager<usint> main_resource_manager;

/**
 * Sprawdzenie autentyczności archiwum!
 */
bool isFilesystemFake() {
	return (strcmp(main_filesystem.getHeader()->author, FILESYSTEM_AUTHOR) != 0);
}

Shader* shaders[HIT_CHARACTER_SHADER + 1];

/**
 * Wczytywanie paczki shaderów!
 */
void loadShadersPack() {
	shaders[WINDOW_SHADOW_SHADER] = new Shader(NULL,
			main_filesystem.getExternalFileContent("shadow_frag_shader.txt"),
			NULL);

	shaders[HIT_CHARACTER_SHADER] = new Shader(NULL,
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
