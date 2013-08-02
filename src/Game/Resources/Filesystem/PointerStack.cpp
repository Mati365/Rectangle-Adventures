/*
 * PointerStack.cpp
 *
 *  Created on: 20-06-2013
 *      Author: mateusz
 */
#include "Filesystem.hpp"
#include "../../Tools/Logger.hpp"

using namespace Filesystem;

PackagePointer::PackagePointer(const char* _label, size_t _offset) :
				label(const_cast<char*>(_label)),
				offset(_offset) {
}

size_t PackagePointer::write(FILE* file) {
	if (!IO::writeString(file, label) || !IO::write<size_t>(file, offset)) {
		return 0;
	}
	return getLength();
}

bool PackagePointer::read(FILE* file) {
	if (!file) {
		return false;
	}
	label = const_cast<char*>(IO::readString(file));
	offset = IO::read<size_t>(file);
	return true;
}

//-----------------------------

PackagePointer* PointerStack::getPointer(const char* label) {
	for (auto iter = pointers.begin(); iter != pointers.end(); ++iter) {
		if (strcmp(iter->label, label) == 0) {
			return &*iter;
		}
	}
	return NULL;
}

size_t PointerStack::write(FILE* file) {
	size_t length = sizeof(int);
	IO::write<int>(file, pointers.size());
	for (auto iter = pointers.begin(); iter != pointers.end(); ++iter) {
		length += iter->write(file);
	}
	return length;
}

size_t PointerStack::getLength() {
	size_t length = sizeof(int);
	for (auto iter = pointers.begin(); iter != pointers.end(); ++iter) {
		length += iter->getLength();
	}
	return length;
}

bool PointerStack::read(FILE* file) {
	if (!file) {
		return false;
	}
	pointers.clear();
	size_t count = IO::read<int>(file);
	//
	for (usint i = 0; i < count; ++i) {
		PackagePointer ptr;
		ptr.read(file);
		pointers.push_back(ptr);
	}
	if (count != pointers.size()) {
		pointers.clear();
		count = 0;
		return false;
	}
	return true;
}

