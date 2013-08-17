/*
 * ShadersPack.cpp
 *
 *  Created on: 17-08-2013
 *      Author: mateusz
 */
#include "Resources.hpp"

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
	logEvent(Logger::LOG_INFO, "Pomyślnie wczytano paczkę shaderów!");
}

void unloadShadersPack() {
	for (auto* shader : shaders) {
		delete shader;
	}
}

