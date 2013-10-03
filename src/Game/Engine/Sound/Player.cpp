/*
 * Player.cpp
 *
 *  Created on: 08-08-2013
 *      Author: mateusz
 */
#include "Sounds.hpp"

using namespace std;

using namespace Sound;

/** Wczytywanie pliku wav z systemu plikow */
sf::SoundBuffer* Player::loadSound(const char* label) {
	size_t filesize;
	char* buffer = nullptr;

	/** Pobieranie pointeru filesystemu */
#ifdef FILESYSTEM_USAGE
	FILE* fp = main_filesystem.getExternalFile(label, &filesize);
	buffer = IO::getFileContent(fp, filesize);

#else
	const char* path = ("mobs/" + (string) label).c_str();

	buffer = IO::getFileContent(path);
	filesize = IO::getFileLength(path);
#endif

	if (!buffer || filesize == 0) {
		return nullptr;
	}

	/** Wczytywanie */
	sf::SoundBuffer* sound_buffer = new sf::SoundBuffer();
	if (!sound_buffer->LoadFromMemory(buffer, filesize)) {
		logEvent(Logger::LOG_ERROR, "Nie zaladowalem dzwieku!");
	}

	/** Czyszczenie */
	delete[] buffer;
	return sound_buffer;
}

/** Odtwarzanie pliku wav */
sf::Sound* Player::generateBuffer(sf::SoundBuffer* buffer, float volume,
		bool loop) {
	sf::Sound* sound = new sf::Sound;

	sound->SetBuffer(*buffer);
	sound->SetLoop(loop);
	sound->SetVolume(volume);

	return sound;
}

