/*
 * Package.cpp
 *
 *  Created on: 20-06-2013
 *      Author: mateusz
 */
#ifndef OS_WINDOWS
#include <unistd.h>
#else
#include <windows.h>
#endif

#include <sys/types.h>

#include "../../Tools/Converter.hpp"
#include "Filesystem.hpp"

using namespace Filesystem;

/** Logowanie procesu wczytywania */
#define LOADING_LOG

Package::Package(const char* path, const char* author) :
				file_path(Convert::getDynamicValue(path)),
				header(author),
				last_file_ptr(0) {
	file = fopen(path, "r+b");
	if (!file) {
		file = fopen(path, "w+b");
		
		/** Tworzenie pustego pliku z szablonu! */
		if (createSkel())
			logEvent(
					Logger::LOG_WARNING,
					"Podany plik nie istnieje, utworzono pusty plik!");
	}
	length = IO::getFileLength(file);
	
	/** Odczytywanie pliku! */
	if (read(file)) {
		logEvent(Logger::LOG_INFO, "Plik otworzony sukcesem!");
	}
}

bool Package::edit(usint operation, const char* label, FilePackage* object) {
	if (!file) {
		return false;
	}
	
	switch (operation) {
		case ARCH_WRITE:
			writeObject(label, object);
			break;
			/**
			 *
			 */
		case ARCH_READ:
			readObject(label, object);
			break;
			/**
			 *
			 */
		case ARCH_DELETE:
			deleteObject(label);
			break;
			/**
			 *
			 */
		default:
			return false;
	}
	length = IO::getFileLength(file); // odswiezanie dllugosci pliku

	return true;
}

/**
 * Odczytywanie obiektu!
 */
bool Package::readObject(const char* label, FilePackage* object) {
	if (!object) {
		logEvent(Logger::LOG_ERROR, "FixMe!!! Plz");
		return false;
	}
	
	PackagePointer* pointer = pointer_stack.getPointer(label);
	if (!pointer) {
		logEvent(Logger::LOG_ERROR, "Nie znaleziono obiektu!");
		return false;
	}
	
	fseek(file, pointer->offset, SEEK_SET);
	object->read(file);
	rewind(file);
	
	return true;
}

/** Zapis interface */
bool Package::writeObject(const char* label, FilePackage* object) {
	if (!object) {
		logEvent(Logger::LOG_ERROR, "NULL'a nie da sie zapisac!");
		return false;
	}
	size_t offset = header.data_length + header.getLength();
	header.data_length += object->getLength();
	//
	pointer_stack.addPointer(PackagePointer(label, offset));
	
	rewind(file); // cofanie sie do poczatku pliku
	header.write(file);
	
	fseek(file, offset, SEEK_SET); // przeskok do bloku danych
	object->write(file);
	
	pointer_stack.write(file); // aktualizacja bloku wskaznikow
	return true;
}

/** Kasowanie pliku po label'u! */
bool Package::deleteObject(const char* label) {
	PackagePointer* pointer = pointer_stack.getPointer(label);
	if (!pointer) {
		return false;
	}
	/**
	 * Przesuwanie pozostalej czesci sekcji danych w lewo,
	 * eksport zmodyfikowanego naglowku i sekcji pointerow
	 *-------------------------------
	 * Obliczanie dlugosci pliku!
	 */
	size_t buffer_length = 0;
	deque<PackagePointer>* pointers = &pointer_stack.pointers;
	
	for (auto iter = pointers->begin(); iter != pointers->end(); ++iter) {
		if ((buffer_length == 0 || &*iter == &*pointer)
				&& iter + 1 >= pointers->end()) {
			buffer_length = header.data_length + header.getLength()
					- iter->offset;
		}
		if (&*iter == &*pointer) {
			iter = pointers->erase(iter);
			if (iter == pointers->end()) {
				break;
			}
		}
		if (buffer_length == 0 && iter->offset > pointer->offset) {
			buffer_length = iter->offset - pointer->offset;
		}
		if (buffer_length != 0) {
			iter->offset -= buffer_length;
		}
	}
	
	/** Przesuwanie bajtow pliku w lewa strone! */
	if (buffer_length == 0) {
		logEvent(Logger::LOG_ERROR, "Uszkodzona referencja do pliku!");
		return false;
	}
	header.data_length -= buffer_length;
	
	/** Wielkosc bufora jest taka sama co usuwanego pliku! */
	size_t rewrite_pos = 0;
	char* buffer = new char[buffer_length];
	memset(buffer, ' ', buffer_length);
	bool stop = false;
	
	while (!feof(file)) {
		/** Przeskakiwanie w miejsce po pliku */
		fseek(file, pointer->offset + buffer_length + rewrite_pos, SEEK_SET);
		fread(buffer, sizeof(char), buffer_length, file);
		
		/** Przeskakiwanie w miejsce pliku */
		fseek(file, pointer->offset + rewrite_pos, SEEK_SET);
		fwrite(buffer, sizeof(char), buffer_length, file);
		if (stop) {
			break;
		}
		
		//
		rewrite_pos += buffer_length;
		if (pointer->offset + rewrite_pos + buffer_length > length) {
			buffer_length = length - (pointer->offset + rewrite_pos);
			stop = true;
		}
	}
	delete[] buffer;
	
	/** Zmiana rozmiaru */
	fflush(file);
	
	/** Zmiana rozmiaru */
#ifndef OS_WINDOWS
	ftruncate(fileno(file), header.getLength() + header.data_length);
#endif
	
	fclose(file);
	file = fopen(file_path, "r+b");
	//
	fseek(file, 0, SEEK_END);
	pointer_stack.write(file);
	//
	fseek(file, 0, SEEK_SET);
	header.write(file);
	return true;
}

/** Wczytywanie zewnetrznego pliku bez jego wypakowania! */
FILE* Package::getExternalFile(const char* _label, size_t* _length) {
	last_file_ptr = ftell(file);
	//
	PackagePointer* pointer = pointer_stack.getPointer(_label);
	//
	if (!pointer) {
		logEvent(Logger::LOG_ERROR, "Nie mogę otworzyć wskazanej paczki!");
		return nullptr;
	}
	
	if (_length) {
		fseek(file, pointer->offset, SEEK_SET);
		fread(_length, sizeof(size_t), 1, file);
	}
	fseek(file, pointer->offset + sizeof(size_t), SEEK_SET);
	
	return file;
}

/** Wczytywanie zawartosci zewnetrznego pliku!  */
char* Package::getExternalFileContent(const char* _label) {
	size_t len = 1; // NIE NULL!!!
	FILE* file = getExternalFile(_label, &len);
	//
	return IO::getFileContent(file, len);
}

/** Wczytywanie ostatnio wczytanego wskazniku do pliku! */
void Package::closeExternalFile() {
	fseek(file, last_file_ptr, SEEK_SET);
}

/** Tworzenie pustego pliku! */
bool Package::createSkel() {
	if (!file) {
		return false;
	}
	header.write(file);
	pointer_stack.write(file);
	return true;
}

bool Package::read(FILE* file) {
	if (!file) {
		return false;
	}
	rewind(file);
	header.read(file);
	if (header.version == BUILD) {
		fseek(file, header.data_length, SEEK_CUR);
		pointer_stack.read(file);
	} else {
		logEvent(Logger::LOG_ERROR, "Wersja archiwum jest niepoprawna!");
		return false;
	}
	return true;
}

size_t Package::write(FILE*) {
	return length;
}
