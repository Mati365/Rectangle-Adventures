/*
 * Filesystem.hpp
 *
 *  Created on: 20-06-2013
 *      Author: mateusz
 */

#ifndef FILESYSTEM_HPP_
#define FILESYSTEM_HPP_
#include <stdio.h>
#include <string.h>
#include <deque>

#include "IO.hpp"

#include "../../Tools/Logger.hpp"

#define BUILD 10
#define CLOSE_FILE(file) fclose(file); \
	file = NULL

typedef unsigned short int usint;

/**
 * UWAGA!
 * + BRAK KOMPRESJII TYPOW PODSTAWOWYCH: TYLKO TYP EKSPORT/IMPORT PLIKU!
 * + FUNKCJE KOMPRESUJACE NIE SA UMIESZCZONE TUTAJ(OSOBNE DLL)
 *
 * SZKIELET PLIKU:
 * [ header ] [     dane     ] [ pointery ]
 */
namespace Filesystem {
	
	/**
	 * Interfejs serializacji obiektOw!
	 * TO NIE PLIK ARCHIWUM!!
	 */
	class FilePackage {
		protected:
			size_t length;

		public:
			virtual bool read(FILE*)=0;

			virtual size_t write(FILE*)=0;
			virtual size_t getLength()=0;

			virtual ~FilePackage() {
			}
	};
	
	/**
	 * Naglowek pliku!
	 * Flagi nie potrzebne!
	 */
	class Header: public FilePackage {
		public:
			char* author;
			int version;

			size_t data_length;

			Header(const char* _author) :
							author(Convert::getDynamicValue(_author)),
							version(BUILD),
							data_length(0) {
			}
			
			virtual size_t getLength() {
				return IO::stringLength(author) + sizeof(int) + sizeof(size_t);
			}
			
			virtual bool read(FILE*);
			virtual size_t write(FILE*);

			~Header() {
				if (author) {
					delete[] author;
				}
			}
	};
	
	/**
	 * Sekcja konteneru offsetOw!
	 * Wskaznik na miejsce w sekcji danych.
	 */
	class PackagePointer: public FilePackage {
		public:
			char* label;
			size_t offset;

			PackagePointer() :
							label(NULL),
							offset(0) {
			}
			PackagePointer(const char*, size_t);

			virtual bool read(FILE*);
			virtual size_t write(FILE*);

			virtual size_t getLength() {
				if (!label) {
					return 0;
				}
				return IO::stringLength(label) + sizeof(int);
			}
	};
	
	/** Stos wskaznikow w archiwum! */
	class PointerStack: public FilePackage {
		public:
			deque<PackagePointer> pointers;

			void addPointer(const PackagePointer& _pointer) {
				pointers.push_back(_pointer);
			}
			
			PackagePointer* getPointer(const char*);

			virtual bool read(FILE*);
			virtual size_t write(FILE*);

			virtual size_t getLength();
	};
	
	/** Zarzadzanie plikiem archiwum  */
	class Package: public FilePackage {
		public:
			enum Operation {
				ARCH_WRITE,
				ARCH_READ,
				ARCH_DELETE
			};

		protected:
			/** Info o pliku */
			FILE* file;
			char* file_path;
			size_t length;

			/** Struktura vfs */
			Header header;
			PointerStack pointer_stack;

			/**
			 * Potrzebne dla funkcji:
			 * + getExternalFile
			 * + closeExternalFile
			 */
			size_t last_file_ptr;

		public:
			Package(const char*, const char*);

			/**
			 * Skakanie po pliku, w razie odczytywania
			 * pliku NA RAZ w 2 oddzielnych funkcjach
			 * trzeba resetowac wskaznik pliku!
			 */
			FILE* getExternalFile(const char*, size_t*);
			char* getExternalFileContent(const char*);

			void closeExternalFile();

			bool edit(usint, const char*, FilePackage*);

			virtual bool read(FILE*);
			virtual size_t write(FILE*);

			Header* getHeader() {
				return &header;
			}
			
			PointerStack* getPointerStack() {
				return &pointer_stack;
			}
			
			char* const getFilePath() const {
				return file_path;
			}
			
			FILE* getFile() {
				return file;
			}
			
			virtual size_t getLength() {
				return length;
			}
			
			void close() {
				if (file) {
					CLOSE_FILE(file);
				}
			}
			
			~Package() {
				close();
				//
				delete[] file_path;
			}
			
		private:
			/** Generowanie pustego szkieletu */
			bool createSkel();

			/** Operacje wejscia/wyjscia! */
			bool readObject(const char*, FilePackage*);
			bool writeObject(const char*, FilePackage*);
			bool deleteObject(const char*);
	};
}

#endif /* FILESYSTEM_HPP_ */
