/*
 * main.cpp
 *
 *  Created on: 23-02-2013
 *      Author: mati
 */
#include "Game/Tools/Logger.hpp"

//#define _ARCHIVER_

#ifndef _ARCHIVER_

#include "Game/Engine/Graphics/Engine.hpp"
#include "Game/Resources/Data/Resources.hpp"

#include <GL/glew.h>
#include <GL/glut.h>

//
#else
//
#include "Game/Resources/Filesystem/Filesystem.hpp"
#include "Game/Resources/Filesystem/Files.hpp"

using namespace Filesystem;
using namespace File;
#endif

/**
 *	Nazwa: Rect Game
 */

int main(int argc, char **argv) {
#ifndef _ARCHIVER_
	glutInit(&argc, argv);
	
	srand(time(NULL));
	
	if (isFilesystemFake()) {
		logEvent(Logger::LOG_ERROR, "Plik archiwum gry nie jest autentyczny!");
		return 1;
	}
	Window::getInstance().init();
#else
	/**
	 * Example: ./archiver NAZWA_ARCHIWUM AUTOR plik1.txt plik2.txt
	 */
	if (argc == 1) {
		logEvent(Logger::LOG_ERROR, "Brak plików do spakowania!");
		return 1;
	}
	logEvent(Logger::LOG_INFO, "Pakowanie plików..");
	//
	Package pack(argv[1], argv[2]);
	for (long i = 3; i < argc; ++i) {
		ExternalFile* file = new ExternalFile(argv[i]);
		// Eksport!
		pack.edit(Package::ARCH_WRITE, argv[i], file);
		delete file;
	}
	//
	logEvent(Logger::LOG_INFO, "Pakowanie przebiegło pomyślnie!");
#endif
	return 0;
}
