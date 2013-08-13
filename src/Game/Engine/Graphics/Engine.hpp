/*
 * Engine.hpp
 *
 *  Created on: 23-02-2013
 *      Author: mati
 */

#ifndef ENGINE_HPP_
#define ENGINE_HPP_
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <string>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

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
		
		/**
		 * Odwrócenie wektora!
		 */
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
	extern Color RED, DARK_RED, GREEN, DARK_GREEN, BLUE, DARK_BLUE, BLACK,
			WHITE, GRAY, YELLOW, ORANGE, PURPLE;
	
	/**
	 * Prymitywy!
	 */
	extern void drawRect(float, float, float, float, const Color&, float);
	extern void drawFillRect(float, float, float, float, const Color&);
	
	extern void beginStroke(GLushort);
	extern void endStroke();
	
	extern void drawCircle(float, float, float, float, const Color&, float);
	extern void drawLine(float, float, float, float, const Color&, float);
	
	extern void drawTriangle(float, float, float, float, const Color&, float);
	
	/**
	 * Shadery!
	 */
	extern GLuint createShader(GLuint, const GLchar*);
	
	class Shader {
		private:
			GLuint program_object;
			/**
			 * 3 typy shaderów!
			 */
			GLuint vertex_shader;
			GLuint fragment_shader; // dawniej pixel shader
			GLuint geometry_shader;

		public:
			Shader() :
							program_object(0),
							vertex_shader(0),
							fragment_shader(0),
							geometry_shader(0) {
			}
			
			Shader(GLchar*, GLchar*, GLchar*);

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

			/**
			 * Wywołuwanie shaderu!
			 */
			void begin();
			void end();

			/**
			 * Uniformy!
			 */
			void setUniform1f(const char*, float);
			void setUniform4fv(const char*, float*, size_t);
			void setUniform2f(const char*, float, float);

			~Shader();

		protected:
			void linkShader();
	};
}

/**
 * Silnik graficzny!
 */
namespace Engine {
	class Window;
	class Renderer {
		public:
			/**
			 * Jeśli obiekt został wyłączony
			 * to zostaje zdjęty ze sceny,
			 * kasuje go potem obiekt obserwujący
			 */
			bool destroyed;
			/**
			 * Obiekty stworzone dynamicznie,
			 * nie kontrolowane przez żaden
			 * inny obiekt np. pocisk
			 */
			bool dynamically_allocated;

			//
			Renderer() :
							destroyed(false),
							dynamically_allocated(false) {
			}
			
			virtual void drawObject(Window*) = 0;
			virtual ~Renderer() {
				/**
				 *
				 */
			}
	};
	/**
	 * Główne okno aplikacji obsługujące pętle gry!
	 */
	extern bool window_opened;
	class Window {
		private:
			SDL_Surface* screen;
			Vector<usint> bounds;

		public:
			Window(const Vector<usint>&, const string&);

			void init();

			const Vector<usint>* getBounds() {
				return &bounds;
			}
			
			~Window();

		private:
			bool setupOpenGL();
	};
}

#endif /* ENGINE_HPP_ */
