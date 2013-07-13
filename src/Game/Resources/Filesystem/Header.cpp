/*
 * Header.cpp
 *
 *  Created on: 20-06-2013
 *      Author: mateusz
 */
#include "Filesystem.hpp"

using namespace Filesystem;

size_t Header::write(FILE* file) {
	IO::writeString(file, author);
	IO::write<int>(file, version);
	IO::write<size_t>(file, data_length);
	//
	return getLength();
}

bool Header::read(FILE* file) {
	if (!file) {
		return false;
	}
	if (author) {
		delete[] author;
	}
	author = const_cast<char*>(IO::readString(file));
	version = IO::read<int>(file);
	data_length = IO::read<size_t>(file);
	return true;
}
