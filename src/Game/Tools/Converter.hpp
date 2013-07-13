/*
 * Converter.hpp
 *
 *  Created on: 20-02-2013
 *      Author: mati
 */

#ifndef CONVERTER_HPP_
#define CONVERTER_HPP_
#include <string>
#include <sstream>

using namespace std;

namespace Convert {
	template<typename T>
	T stringTo(const string& str) {
		stringstream buf;
		T num;
		buf << str;
		buf >> num;
		return num;
	}

	template<typename T>
	string toString(T t) {
		ostringstream buf;
		buf << t;
		return buf.str();
	}

	char* getDynamicValue(const char*);
}

#endif /* CONVERTER_HPP_ */
