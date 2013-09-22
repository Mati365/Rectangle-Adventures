/*
 * IO.cpp
 *
 *  Created on: 21-09-2013
 *      Author: bagin_000
 */
#include "IO.hpp"

bool IO::writeString(FILE* file, const char* str) {
	if (!file) {
		return false;
	}
	int len = strlen(str) + 1;
	// Ostatni znak to znak końca linii
	fwrite(&len, sizeof(int), 1, file);
	fwrite(str, sizeof(char), len, file);
	return true;
}

bool IO::writeString(const char* path, const char* str) {
	FILE* file = fopen(path, "w");
	if (!writeString(file, str)) {
		return false;
	}
	fclose(file);
	return true;
}

int IO::stringLength(const char* str) {
	return strlen(str) + sizeof(int) + 1;
}

const char* IO::readString(FILE* file) {
	if (!file) {
		return NULL;
	}
	int len;
	fread(&len, sizeof(int), 1, file);
	char* str = new char[len + 1];
	memset(str, ' ', len + 1);
	fread(str, sizeof(char), len, file);
	//
	return str;
}

const char* IO::readString(const char* path) {
	FILE* file = fopen(path, "r");
	if (!file) {
		return NULL;
	}
	const char* str = readString(file);
	fclose(file);
	//
	return str;
}

char* IO::getFileContent(const char* _path) {
	FILE* file = fopen(_path, "rb");
	//
	char* content = getFileContent(file, 0);
	if (content) {
		fclose(file);
	}
	//
	return content;
}

char* IO::getFileContent(FILE* file, size_t len) {
	if (!file) {
		return nullptr;
	}
	if (len == 0) {
		len = getFileLength(file);
	}
	//
	char* content = new char[len + 1];
	memset(content, 0, len + 1);
	fread(content, len, 1, file);
	//
	return content;
}

bool IO::fileExists(const char* path) {
	FILE* file = fopen(path, "r");
	bool exists = file;
	//
	fclose(file);
	return exists;
}

size_t IO::getFileLength(FILE* file) {
	size_t pos = ftell(file);
	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	fseek(file, pos, SEEK_SET);
	//
	return length;
}

size_t IO::getFileLength(const char* file) {
	FILE* fp = fopen(file, "r");
	size_t len = getFileLength(fp);
	fclose(fp);
	//
	return len;
}
