/*
 * Files.cpp
 *
 *  Created on: 22-06-2013
 *      Author: mateusz
 */
#include "Files.hpp"

using namespace File;

/**
 *  Przenoszenie zawartości z pliku do innego pliku!
 */
bool File::moveBytes(FILE* source, FILE* destination, size_t buffer_size,
		size_t source_length, size_t source_start_position) {
	if (!source || !destination || buffer_size == 0 || source_length == 0) {
		return false;
	}
	bool stop = false;
	char* buffer = new char[buffer_size];
	//
	memset(buffer, ' ', buffer_size);
	while (true) {
		if (ftell(source) - source_start_position + buffer_size
				>= source_length) {
			buffer_size = source_length + source_start_position - ftell(source);
			stop = true;
		}
		fread(buffer, sizeof(char), buffer_size, source);
		fwrite(buffer, sizeof(char), buffer_size, destination);
		if (stop) {
			break;
		}
	}
	delete[] buffer;
	return true;
}
/**
 *
 */

ExternalFile::ExternalFile(const char* _path) :
				path(_path) {
	file = fopen(path, "rb+");
	if (file) {
		length = getLength();
	}
}

/**
 * Odczyt z archiwum, wpisywanie do osobnego pliku!
 */
bool ExternalFile::read(FILE* _file) {
	if (file) {
		CLOSE_FILE(file);
	}
	length = IO::read<size_t>(_file);
	file = fopen(path, "wb+");
	//
	moveBytes(_file, file, BUFFER_SIZE, length, ftell(_file));
	return true;
}

/**
 * Zapis do archiwum!
 */
size_t ExternalFile::write(FILE* _file) {
	length = IO::getFileLength(file);
	//
	IO::write<size_t>(_file, length);
	//
	moveBytes(file, _file, BUFFER_SIZE, length, 0);
	return getLength();
}

size_t ExternalFile::getLength() {
	if (!file) {
		return 0;
	}
	return sizeof(size_t) + IO::getFileLength(file);
}

