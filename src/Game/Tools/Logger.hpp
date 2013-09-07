/*
 * Logger.hpp
 *
 *  Created on: 20-02-2013
 *      Author: mati
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_
#include "../Tools/Converter.hpp"

typedef unsigned short int usint;

/** Logowanie niczego oprócz pasków wczytywania */
#define DEBUG_LOGGER

class Logger {
#define logEvent(type, log) Logger::logOperation(type, log, __FILE__, __LINE__)
		
		/** Do pasku wczytywania */
#define BEGIN_LOADING(s) logEvent(Logger::LOG_PROGRESSBAR_BEGIN, s)
#define END_LOADING() logEvent(Logger::LOG_PROGRESSBAR_END, "")
#define PROGRESS_LOADING() logEvent(Logger::LOG_PROGRESSBAR_PROCENT, "")
		
	public:
		enum Type {
			LOG_ERROR,
			LOG_WARNING,
			LOG_INFO,
			/** Paski stanu */
			LOG_PROGRESSBAR_BEGIN,
			LOG_PROGRESSBAR_PROCENT,
			LOG_PROGRESSBAR_END
		};

	public:
		/** Logowanie wiadomości */
		static void logOperation(usint, const string&, const string&, usint);
};

#endif /* LOGGER_HPP_ */
