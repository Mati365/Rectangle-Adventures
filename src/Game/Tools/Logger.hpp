/*
 * Logger.hpp
 *
 *  Created on: 20-02-2013
 *      Author: mati
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_
#include <iostream>
#include <deque>

#include "../Tools/Converter.hpp"

using namespace std;

typedef unsigned short int usint;

class Logger {
#define logEvent(type, log) Logger::logOperation(type, log, __FILE__, __LINE__)
		
	public:
		enum Type {
			LOG_ERROR, LOG_WARNING, LOG_INFO
		};

	public:
		static void logOperation(usint type, const string& log,
				const string& file, usint row) {
			string buffer = "";
			switch (type) {
				case LOG_ERROR:
					buffer += "ERROR! ";
					break;
					
				case LOG_WARNING:
					buffer += "WARN! ";
					break;
					
				case LOG_INFO:
					buffer += "INFO: ";
					break;
			}
			buffer += file + " in " + Convert::toString<usint>(row)
					+ " line -> " + log;
			//
			cout << buffer << endl;
		}
};

#endif /* LOGGER_HPP_ */
