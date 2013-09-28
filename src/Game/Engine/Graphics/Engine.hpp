/*
 * Engine.hpp
 *
 *  Created on: 23-02-2013
 *      Author: mati
 */

#ifndef ENGINE_HPP_
#define ENGINE_HPP_
#include <GL/glew.h>
#include <GL/gl.h>

#include <SDL/SDL.h>
#include <string>

using namespace std;

typedef unsigned short int usint;

struct Color {
		usint r;
		usint g;
		usint b;
		usint a;

		Color() :
						r(0),
						g(0),
						b(0),
						a(255) {
		}
		
		Color(usint _r, usint _g, usint _b) :
						r(_r),
						g(_g),
						b(_b),
						a(255) {
		}
		
		Color(usint _r, usint _g, usint _b, usint _a) :
						r(_r),
						g(_g),
						b(_b),
						a(_a) {
		}
		
		bool operator==(const Color& col) {
			if (col.a == a && col.r == r && col.g == g && col.b == b) {
				return true;
			}
			return false;
		}
};

template<typename T>
struct Vector {
		T x;
		T y;

		Vector() :
						x(0),
						y(0) {
		}
		
		Vector(T _x, T _y) :
						x(_x),
						y(_y) {
		}
		
		/** Odwrocenie wektora! */
		inline void invert() {
			x = -x;
			y = -y;
		}
		
		inline Vector<T>& operator-=(const Vector<T>& right) {
			x -= right.x;
			y -= right.y;
			return *this;
		}
		
		inline Vector<T>& operator+=(const Vector<T>& right) {
			x += right.x;
			y += right.y;
			return *this;
		}
};

/**
 * Wrapper na podstawowe funkcje OpenGL
 */
namespace oglWrapper {
	/** Paleta barw */
	extern Color RED, DARK_RED, GREEN, DARK_GREEN, BLUE, DARK_BLUE, BLACK,
			WHITE, GRAY, YELLOW, ORANGE, PURPLE;
	
	/** Prymitywy! */
	extern void drawRect(float, float, float, float, const Color&, float);
	extern void drawFillRect(float, float, float, float, const Color&);
	
	extern void beginStroke(GLushort);
	extern void endStroke();
	
	extern void drawCircle(float, float, float, float, const Color&, float);
	extern void drawLine(float, float, float, float, const Color&, float);
	
	extern void drawTriangle(float, float, float, float, const Color&, float);
	
	/** Shadery! */
	extern usint createShader(usint, const char*);
	
	class Shader {
		private:
			GLuint program_object;
			/** 3 typy shaderow! */
			GLuint vertex_shader;
			GLuint fragment_shader; // dawniej pixel shader
			GLuint geometry_shader;

			/** Test kompilacji */
			bool compiled;

		public:
			Shader() :
							program_object(0),
							vertex_shader(0),
							fragment_shader(0),
							geometry_shader(0),
							compiled(true) {
			}
			
			Shader(char*, char*, char*);

			bool isCompiled() const {
				return compiled;
			}
			
			GLuint getShaderProgram() const {
				return program_object;
			}
			
			GLuint getVertexShader() const {
				return vertex_shader;
			}
			
			GLuint getFragmentShader() const {
				return fragment_shader;
			}
			
			static GLint getLastShader() {
				GLint last_program;
				glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
				//
				return last_program;
			}
			
			/** Wywolywanie shaderu! */
			void begin();
			void end();

			/** Uniformy! */
			void setUniform1f(const char*, float);
			void setUniform2f(const char*, float, float);
			void setUniform3f(const char*, float, float, float);
			void setUniform4f(const char*, float, float, float, float);

			~Shader();

		protected:
			bool linkShader();
	};
}

/** Silnik graficzny! */
namespace Engine {
	/** Rozdzielczosc ekranu */
	extern Vector<float> screen_bounds;
	
	class Window;
	class Renderer {
		public:
			/**
			 * Jesli obiekt zostal wylczony
			 * to zostaje zdjety ze sceny,
			 * kasuje go potem obiekt obserwujacy
			 */
			bool destroyed;

			/**
			 * Jesli ma obserwatora to nie jest
			 * kasowany w silniku fizycznym
			 */
			bool with_observer;

			Renderer() :
							destroyed(false),
							with_observer(false) {
			}
			
			virtual void drawObject(Window*) = 0;
			virtual ~Renderer() {
				/**
				 *
				 */
			}
	};
	/**
	 * Glowne okno aplikacji onslugujace petle gry!
	 * + Singleton
	 */
	struct WindowConfig {
			enum {
				WINDOW_OPENED,
				RESOLUTION_CHANGED,
				WITH_SHADERS,
				WSAD_CONTROLS,
				FULLSCREEN
			};

			/** Flagi okna */
			bool flag[FULLSCREEN + 1];

			WindowConfig() {
				for (usint i = 0; i < FULLSCREEN + 1; ++i) {
					flag[i] = false;
					if (i >= WSAD_CONTROLS) {
						flag[i] = true;
					}
				}
			}

			/** Dodawanie configu */
			void putConfig(usint type, bool value) {
				flag[type] = value;
			}
	};

	extern WindowConfig window_config;
	
	class Window {
		private:
			SDL_Surface* screen;

			/** Konstruktor */
			Window(const string&);
		public:
			
			/** Inicjacja okna */
			void init();

			/** Singleton */
			static Window& getInstance() {
				static Window win("Okno");
				//
				return win;
			}
			
			/** Wyliczanie natywnej rozdzielczosci ekranu */
			static Vector<float> getNativeResolution();

			~Window();

		private:
			/** Instalacja OpenGL */
			bool setupOpenGL();
	};
	
	/** Opcje myszki */
	extern void hideMouse();
	extern void showMouse();
}

#endif /* ENGINE_HPP_ */
