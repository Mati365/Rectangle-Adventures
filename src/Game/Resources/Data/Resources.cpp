/*
 * Resources.cpp
 *
 *  Created on: 13 lip 2013
 *      Author: mateusz
 */
#include "Resources.hpp"

Package main_filesystem(
FILESYSTEM_PACKAGE,
						FILESYSTEM_AUTHOR);
ResourceManager<usint> main_resource_manager;

/**
 * Sprawdzenie autentyczności archiwum!
 */
bool isFilesystemFake() {
	return (strcmp(main_filesystem.getHeader()->author, FILESYSTEM_AUTHOR) != 0);
}
