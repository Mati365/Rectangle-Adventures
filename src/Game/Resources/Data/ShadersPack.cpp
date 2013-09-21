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
	shaders[WINDOW_SHADOW_SHADER] = new Shader(NULL,
#ifdef FILESYSTEM_USAGE
			main_filesystem.getExternalFileContent("shadow_frag_shader.txt"),
#else
			IO::getFileContent("mobs/shadow_frag_shader.txt"),
#endif
			NULL);
	
	shaders[HIT_CHARACTER_SHADER] = new Shader(NULL,
#ifdef FILESYSTEM_USAGE
			main_filesystem.getExternalFileContent("hit_frag_shader.txt"),
#else
			IO::getFileContent("mobs/hit_frag_shader.txt"),
#endif
			NULL);
	logEvent(Logger::LOG_INFO, "Pomyślnie wczytano paczkę shaderów!");
}

void unloadShadersPack() {
	for (auto& shader : shaders) {
		safe_delete<Shader>(shader);
	}
}

