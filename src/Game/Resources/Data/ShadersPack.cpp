/*
 * ShadersPack.cpp
 *
 *  Created on: 17-08-2013
 *      Author: mateusz
 */
#include "Resources.hpp"

Shader* shaders[HIT_CHARACTER_SHADER + 1];

/** Wczytywanie paczki shaderow! */
void loadShadersPack() {
	shaders[WINDOW_SHADOW_SHADER] = new Shader(NULL,
#ifdef FILESYSTEM_USAGE
			main_filesystem.getExternalFileContent("shadow_frag_shader.txt"),
#else
			IO::getFileContent("mobs/shadow_frag_shader.txt"),
#endif
			nullptr);
	
	shaders[HIT_CHARACTER_SHADER] = new Shader(NULL,
#ifdef FILESYSTEM_USAGE
			main_filesystem.getExternalFileContent("hit_frag_shader.txt"),
#else
			IO::getFileContent("mobs/hit_frag_shader.txt"),
#endif
			nullptr);
	/** Test poprawnosci kompilacji */
	for (usint i = 0; i < HIT_CHARACTER_SHADER + 1; ++i) {
		if (!shaders[i]->isCompiled()) {
			logEvent(Logger::LOG_INFO, "Shadery nie skompilowane!");

			window_config.putConfig(WindowConfig::WITH_SHADERS, false);
			return;
		}
	}
	logEvent(Logger::LOG_INFO, "Pomyslnie wczytano paczke shaderow!");
}

void unloadShadersPack() {
	for (auto& shader : shaders) {
		safe_delete<Shader>(shader);
	}
}

