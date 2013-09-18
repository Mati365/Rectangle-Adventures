/*
 * Converter.cpp
 *
 *  Created on: 03-07-2013
 *      Author: mateusz
 */

#include <string.h>

#include "Converter.hpp"

char* Convert::getDynamicValue(const char* str) {
	if (!str || strlen(str) == 0) {
		return nullptr;
	}
	char* _str = new char[strlen(str) + 1];
	strcpy(_str, str);
	return _str;
}

