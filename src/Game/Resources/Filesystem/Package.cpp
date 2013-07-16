/*
 * Package.cpp
 *
 *  Created on: 20-06-2013
 *      Author: mateusz
 */
#include <unistd.h>
#include <sys/types.h>

#include "../../Tools/Converter.hpp"

#include "Filesystem.hpp"

using namespace Filesystem;

Package::Package(const char* path, const char* author) :
		file_path(Convert::getDynamicValue(path)),
		header(author),
		last_file_ptr(0) {
	file = fopen(path, "rb+");
	if (!file) {
		file = fopen(path, "wb+");
		/**
		 * Tworzenie pustego pliku z szablonu!
		 */
		if (createSkel())
			logEvent(
					Logger::LOG_WARNING,
					"Podany plik nie istnieje, utworzono pusty plik!");
	}
	length = IO::getFileLength(file);
	/**
	 * Odczytywanie pliku!
	 */
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
	length = IO::getFileLength(file); // odświeżanie długości pliku!
	return true;
}

/**
 * Odczytywanie obiektu!
 */
bool Package::readObject(const char* label, FilePackage* object) {
	if (!object) {
		logEvent(
				Logger::LOG_ERROR, "NULL w object to zuo, napraw to prosze :<");
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

/**
 * Zapis interface'u!
 */
bool Package::writeObject(const char* label, FilePackage* object) {
	if (!object) {
		logEvent(Logger::LOG_ERROR, "NULL'a nie da sie zapisac!");
		return false;
	}
	size_t offset = header.data_length + header.getLength();
	header.data_length += object->getLength();
	//
	pointer_stack.addPointer(PackagePointer(label, offset));

	rewind(file); // cofanie się do początku
	header.write(file);

	fseek(file, offset, SEEK_SET); // przeskok do bloku danych
	object->write(file);

	pointer_stack.write(file); // aktualizacja bloku wskaźników
	return true;
}

/**
 * Kasowanie pliku po label'u!
 */
bool Package::deleteObject(const char* label) {
	PackagePointer* pointer = pointer_stack.getPointer(label);
	if (!pointer) {
		return false;
	}
	/**
	 * Przesuwanie pozostałej części sekcji danych w lewo,
	 * eksport zmodyfikowanego nagłówku i sekcji pointerów
	 *-------------------------------
	 * Obliczanie długości pliku!
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

	/**
	 * Przesuwanie bajtów w pliku w lewą stronę!
	 */
	if (buffer_length == 0) {
		logEvent(Logger::LOG_ERROR, "Uszkodzona referencja do pliku!");
		return false;
	}
	header.data_length -= buffer_length;
	/**
	 * Wielkość bufora jest taka sama co usuwanego pliku!
	 */
	size_t rewrite_pos = 0;
	char* buffer = new char[buffer_length];
	memset(buffer, ' ', buffer_length);
	bool stop = false;

	while (!feof(file)) {
		// Przeskakiwanie w miejsce po pliku
		fseek(file, pointer->offset + buffer_length + rewrite_pos, SEEK_SET);
		fread(buffer, sizeof(char), buffer_length, file);
		// Przeskakiwanie w miejsce pliku
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

	// Zmiana rozmiaru
	fflush(file);
	ftruncate(fileno(file), header.getLength() + header.data_length);
	fclose(file);
	file = fopen(file_path, "rb+");
	//
	fseek(file, 0, SEEK_END);
	pointer_stack.write(file);
	//
	fseek(file, 0, SEEK_SET);
	header.write(file);
	return true;
}

/**
 * Wczytywanie zewnętrznego pliku bez jego wypakowania!
 */
FILE* Package::getExternalFile(const char* _label, size_t* _length) {
	last_file_ptr = ftell(file);
	//
	PackagePointer* pointer = pointer_stack.getPointer(_label);
	//
	if (!pointer) {
		logEvent(Logger::LOG_ERROR, "Nie mogę otworzyć wskazanego pliku!");
		return NULL;
	}
	if (length) {
		fseek(file, pointer->offset, SEEK_SET);
		fread(&length, sizeof(size_t), 1, file);
	}
	fseek(file, pointer->offset + sizeof(size_t), SEEK_SET);
	return file;
}

/**
 * Wczytywanie ostatnio wczytanego wskaźniku do pliku!
 */
void Package::closeExternalFile() {
	fseek(file, last_file_ptr, SEEK_SET);
}

/**
 * Tworzenie pustego pliku!
 */
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
