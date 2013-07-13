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
		static bool writeString(FILE* file, const char* str) {
			if (!file) {
				return false;
			}
			int len = strlen(str) + 1;
			// Ostatni znak to znak końca linii
			fwrite(&len, sizeof(int), 1, file);
			fwrite(str, sizeof(char), len, file);
			return true;
		}

		static bool writeString(const char* path, const char* str) {
			FILE* file = fopen(path, "w");
			if (!writeString(file, str)) {
				return false;
			}
			fclose(file);
			return true;
		}
		/**
		 * Wielkość string'u zapisywanego do pliku!
		 */
		static int stringLength(const char* str) {
			return strlen(str) + sizeof(int) + 1;
		}
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
		static const char* readString(FILE* file) {
			if (!file) {
				return NULL;
			}
			int len;
			fread(&len, sizeof(int), 1, file);
			char* str = new char[len];
			memset(str, ' ', len);
			fread(str, sizeof(char), len, file);
			//
			return str;
		}

		static const char* readString(const char* path) {
			FILE* file = fopen(path, "rb");
			if (!file) {
				return NULL;
			}
			const char* str = readString(file);
			fclose(file);
			//
			return str;
		}
		/**
		 *
		 */
		static char* getFileContent(FILE* file) {
			size_t len = getFileLength(file);
			//
			char* content = new char[len];
			fread(content, sizeof(char), len, file);
			return content;
		}
		/**
		 *
		 */
		static bool fileExists(const char* path) {
			FILE* file = fopen(path, "r");
			bool exists = file;
			fclose(file);
			return exists;
		}
		/**
		 *
		 */
		static size_t getFileLength(FILE* file) {
			size_t pos = ftell(file);
			fseek(file, 0, SEEK_END);
			size_t length = ftell(file);
			fseek(file, pos, SEEK_SET);
			return length;
		}
};

#endif /* IO_HPP_ */
