/*
 * Shaders.cpp
 *
 *  Created on: 31 lip 2013
 *      Author: mateusz
 */
#include <GL/glew.h>

#include "../../Tools/Logger.hpp"
#include "../../Resources/Filesystem/IO.hpp"

#include "Engine.hpp"

using namespace oglWrapper;

//--------------------------

GLuint oglWrapper::createShader(const GLuint type, const GLchar* _text) {
	if (strlen(_text) == 0) {
		logEvent(Logger::LOG_ERROR, "Pusty shader!");
		return 0;
	}
	GLuint shader_id = glCreateShader(type);
	GLint len = strlen(_text);
			
	// Tworzenie
	glShaderSource(shader_id, 1, &_text, &len);
	
	// Kompilacja
	glCompileShader(shader_id);
	
	// Sprawdzenie kompilacji
	GLint compiled;
	glGetObjectParameterivARB(shader_id, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		logEvent(Logger::LOG_ERROR, "Nie mogę skompilować shaderu!");
	}
	
	//
	return shader_id;
}

//--------------------------

/**
 * Wczytywanie shaderu!
 */
Shader::Shader(GLchar* _vertex, GLchar* _fragment, GLchar* _geometry) {
	program_object = glCreateProgram();
	//
	if (_vertex) {
		vertex_shader = createShader(GL_VERTEX_SHADER, _vertex);
		delete[] _vertex;
	}
	if (_fragment) {
		fragment_shader = createShader(GL_FRAGMENT_SHADER, _fragment);
		delete[] _fragment;
	}
	if (_geometry) {
		geometry_shader = createShader(GL_GEOMETRY_SHADER, _geometry);
		delete[] _geometry;
	}
	//
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
	glUniform4fv(glGetUniformLocation(program_object, name), count, values);
}

/**
 * Linkowanie shaderu!
 */
void Shader::linkShader() {
	// Linkowanie!
	if (vertex_shader) {
		glAttachShader(program_object, vertex_shader);
	}
	if (fragment_shader) {
		glAttachShader(program_object, fragment_shader);
	}
	if (geometry_shader) {
		glAttachShader(program_object, geometry_shader);
	}
	glLinkProgram(program_object);
	
	// Sprawdzenie linkowania!
	GLint linked;
	glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
	//
	if (!linked) {
		logEvent(Logger::LOG_ERROR, "Nie mogłem zlinkować shaderu!");
	}
}

/**
 * Kasowanie shaderów!
 */
Shader::~Shader() {
	glUseProgram(0);
	glDeleteProgram(program_object);
}

