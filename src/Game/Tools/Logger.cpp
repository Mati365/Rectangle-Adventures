/*
 * Logger.cpp
 *
 *  Created on: 29-08-2013
 *      Author: mateusz
 */
#include <iostream>

#include "Logger.hpp"

using namespace std;

void Logger::logOperation(usint type, const string& log, const string& file,
		usint row) {
	string buffer = "";
	switch (type) {
		/**
		 *
		 */
		case LOG_ERROR:
			buffer += "ERROR! ";
			break;
			
			/**
			 *
			 */
		case LOG_WARNING:
			buffer += "WARN! ";
			break;
			
			/**
			 *
			 */
		case LOG_INFO:
			buffer += "INFO: ";
			break;
	}
	if (type <= LOG_INFO) {
#ifndef DEBUG_LOGGER
		if (!log.empty()) {
			buffer += file + " in " + Convert::toString<usint>(row) + " line -> " + log;
			//
			cout << buffer << endl;
		}
#endif
	} else {
		switch (type) {
			/**
			 *
			 */
			case LOG_PROGRESSBAR_BEGIN:
				cout << log + " [ ";
				break;
				
				/**
				 *
				 */
			case LOG_PROGRESSBAR_PROCENT:
				cout << "#";
				break;
				
				/**
				 *
				 */
			case LOG_PROGRESSBAR_END:
				cout << " ] 100% " << endl;
				break;
		}
	}
}

