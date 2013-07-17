/*
 * Shaders.cpp
 *
 *  Created on: 30-06-2013
 *      Author: mateusz
 */
#include <GL/glew.h>

#include "../../Tools/Logger.hpp"
#include "../../Resources/Filesystem/IO.hpp"

#include "Engine.hpp"

using namespace oglWrapper;

//--------------------------

GLuint oglWrapper::createShader(GLuint type, FILE* file, size_t length) {
	if (!file) {
		logEvent(Logger::LOG_ERROR, "Nie znaleziono pliku shadera!");
		return 0;
	}
	const GLchar* source = IO::getFileContent(file, length);
	GLint len = IO::getFileLength(file);
	GLuint shader_id = glCreateShader(type);

	// Tworzenie
	glShaderSource(shader_id, 1, &source, &len);

	// Kompilacja
	glCompileShader(shader_id);

	// Sprawdzenie kompilacji
	GLint compiled;
	glGetObjectParameterivARB(shader_id, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		logEvent(Logger::LOG_ERROR, "Nie mogę skompilować shaderu!");
	}

	// Zamykanie pliku i powrót!
	fclose(file);
	delete source;
	return shader_id;
}

//--------------------------

Shader::Shader(FILE* _vertex, size_t _vertex_length, FILE* _fragment,
	size_t _fragment_length) {
	loadShader(_vertex, _vertex_length, _fragment, _fragment_length);
}

void Shader::loadShader(FILE* _vertex, size_t _vertex_length, FILE* _fragment,
	size_t _fragment_length) {
	program_object = glCreateProgram();
	vertex_shader = createShader(
	GL_VERTEX_SHADER, _vertex, _vertex_length);
	fragment_shader = createShader(
	GL_FRAGMENT_SHADER, _fragment, _fragment_length);
	linkShader();
}

/**
 * Używanie shaderu!
 */
void Shader::begin() {
	glUseProgram(program_object);
}

void Shader::end() {
	glUseProgram(0);
}

/**
 * Ustawienie uniformów!
 */
void Shader::setUniform1f(const char* name, float value) {
	GLint loc = glGetUniformLocation(program_object, name);
	if (loc != -1) {
		glUniform1f(loc, value);
	} else {
		logEvent(Logger::LOG_ERROR, "Brak uniformu!");
	}
}

void Shader::setUniform4fv(const char* name, float* values, size_t count) {
	glUniform4fv(
	glGetUniformLocation(program_object, name), count, values);
}

/**
 * Linkowanie shaderu!
 */
void Shader::linkShader() {
	// Linkowanie!
	glAttachShader(program_object, vertex_shader);
	glAttachShader(program_object, fragment_shader);
	glLinkProgram(program_object);

	// Sprawdzenie linkowania!
	GLint linked;
	glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
	if (!linked) {
		logEvent(Logger::LOG_ERROR, "Nie mogłem zlinkować shaderu!");
	}
}

Shader::~Shader() {
	glUseProgram(0);
	glDeleteProgram(program_object);
}
