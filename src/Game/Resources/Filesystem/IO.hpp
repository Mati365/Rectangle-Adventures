/*
 * IO.hpp
 *
 *  Created on: 20-06-2013
 *      Author: mateusz
 */

#ifndef IO_HPP_
#define IO_HPP_
#include <stdio.h>
#include <string.h>

class IO {
	public:
		/**
		 * Zapisywanie binarnie!
		 */
		template<typename T>
		static bool write(FILE* file, const T& variable) {
			if (!file) {
				return false;
			}
			fwrite(&variable, sizeof(variable), 1, file);
			return true;
		}
		/**
		 *
		 */
		template<typename T>
		static bool write(const char* path, const T& variable) {
			FILE* file = fopen(path, "wb+");
			if (!file) {
				return false;
			}
			fwrite(&variable, sizeof(variable), 1, file);
			fclose(file);
			return true;
		}
		/**
		 * Zapisywanie string!
		 */
		static bool writeString(FILE*, const char*);

		static bool writeString(const char*, const char*);

		/**
		 * Wielkość string'u zapisywanego do pliku!
		 */
		static int stringLength(const char*);

		/**
		 * Odczyt
		 */
		template<typename T>
		static T read(FILE* file) {
			T type;
			if (!file) {
				return type;
			}
			fread(&type, sizeof(T), 1, file);
			return type;
		}
		
		template<typename T>
		static T read(const char* path) {
			FILE* file = fopen(path, "rb");
			T type;
			if (!file) {
				return type;
			}
			fread(&type, sizeof(T), 1, file);
			fclose(file);
			return type;
		}
		
		/**
		 * Odczytywanie string!
		 */
		static const char* readString(FILE*);

		static const char* readString(const char*);

		/**
		 *
		 */
		static char* getFileContent(const char*);

		static char* getFileContent(FILE*, size_t);

		/**
		 *
		 */
		static bool fileExists(const char*);

		/**
		 *
		 */
		static size_t getFileLength(FILE*);

		/**
		 *
		 */
		static size_t getFileLength(const char*);
};

#endif /* IO_HPP_ */
