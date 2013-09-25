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

usint oglWrapper::createShader(usint type, const char* _text) {
	if (strlen(_text) == 0) {
		logEvent(Logger::LOG_ERROR, "Pusty shader!");
		return 0;
	}
	glewInit();
	
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

/** Wczytywanie shaderu */
Shader::Shader(GLchar* _vertex, GLchar* _fragment, GLchar* _geometry) {
	/** Intel GMA wywala sie w funkcji glCreateProgram nieraz */
	try {
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
		compiled = linkShader();
	} catch (exception& e) {
		compiled = false;
	}
}

/** Rozpoczynanie shaderu! */
void Shader::begin() {
	glUseProgram(program_object);
}

void Shader::end() {
	glUseProgram(0);
}

/** UNIFORMY!!! */
void Shader::setUniform1f(const char* name, float value) {
	GLint loc = glGetUniformLocation(program_object, name);
	if (loc != -1) {
		glUniform1f(loc, value);
	} else {
		logEvent(Logger::LOG_ERROR, "Brak uniformu!");
	}
}

void Shader::setUniform3f(const char* name, float args1, float args2,
		float args3) {
	glUniform3f(
			glGetUniformLocation(program_object, name),
			args1,
			args2,
			args3);
}

void Shader::setUniform4f(const char* name, float args1, float args2,
		float args3, float args4) {
	glUniform4f(
			glGetUniformLocation(program_object, name),
			args1,
			args2,
			args3,
			args4);
}

void Shader::setUniform2f(const char* name, float arg1, float arg2) {
	glUniform2f(glGetUniformLocation(program_object, name), arg1, arg2);
}

/** Linkowanie shaderu! */
bool Shader::linkShader() {
	/** Linkowanie! */
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
	
	/** Sprawdzenie linkowania! */
	GLint linked;
	glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
	//
	if (!linked) {
		logEvent(Logger::LOG_ERROR, "Nie moglem zlinkowac shaderu!");
		return false;
	}
	return true;
}

/** Kasowanie shaderu */
Shader::~Shader() {
	if (compiled) {
		glUseProgram(0);
		glDeleteProgram(program_object);
	}
}

