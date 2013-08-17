/*
 * Save.cpp
 *
 *  Created on: 16-08-2013
 *      Author: mateusz
 */
#include "Resources.hpp"

/**
 * Export save
 */
void exportSave() {
	SaveManager::getInstance().writeToFilesystem(&main_filesystem);
}

/**
 * Odczyt save
 */
void importSave() {
	SaveManager::getInstance().readFromFilesystem(&main_filesystem);
}

/**
 * Menedżer sejvów
 */
bool SaveManager::writeToFilesystem(Package* _filesystem) {
	if (!_filesystem || !save) {
		return false;
	}
	_filesystem->edit(Package::ARCH_DELETE, file_name, NULL);
	return _filesystem->edit(Package::ARCH_WRITE, file_name, save);
}

bool SaveManager::readFromFilesystem(Package* _filesystem) {
	if (!_filesystem || !save) {
		return false;
	}
	save->clear(); // na wszelki wypadek
	return _filesystem->edit(Package::ARCH_READ, file_name, save);
}

/////////////////////////////////////////////

/** Odczyt sejwa */
bool Save::read(FILE* _file) {
	for (auto& stat : stats) {
		stat = IO::read<size_t>(_file);
	}
	//
	return true;
}

/** Zapis sejwa */
size_t Save::write(FILE* _file) {
	for (auto& stat : stats) {
		IO::write<usint>(_file, stat);
	}
	//
	return getLength();
}

/** Czyszczenie save */
void Save::clear() {
	for (auto& stat : stats) {
		stat = 0;
	}
}
