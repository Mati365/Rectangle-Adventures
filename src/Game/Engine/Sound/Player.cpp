/*
 * Player.cpp
 *
 *  Created on: 08-08-2013
 *      Author: mateusz
 */
#include "Sounds.hpp"

using namespace std;

using namespace Sound;

/**
 * Wzorzec singleton!
 */
Player::Player() :
				audio_rate(22050),
				audio_format(AUDIO_S16SYS),
				audio_channels(2),
				audio_buffers(1024) {
	openMixAudio();
}

/**
 * Wczytywanie pliku wav z systemu plików
 */
Mix_Chunk* Player::loadSound(const char* label) {
	size_t filesize;
	char* buffer = nullptr;
	//
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
	
	SDL_RWops* rw = SDL_RWFromMem(buffer, filesize + 1);
	Mix_Chunk* sound = Mix_LoadWAV_RW(rw, 1);
	
	delete[] buffer;
	//
	return sound;
}

/**
 * Odtwarzanie pliku wav
 */
void Player::playChunk(Mix_Chunk* sound, float volume) {
	int channel = Mix_PlayChannel(-1, sound, 0);
	if (channel == -1) {
		fprintf(stderr, "Nie mogę odtworzyć dźwięku: %s\n", Mix_GetError());
	} else {
		Mix_Volume(channel, volume);
	}
}

/**
 * Zamykanie pliku
 */
void Player::closeChunk(Mix_Chunk* sound) {
	Mix_FreeChunk(sound);
}

/**
 * Otwieranie mix
 */
void Player::openMixAudio() {
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)
			!= 0) {
		fprintf(stderr, "Nie mogę zainicjować dźwięku: %s\n", Mix_GetError());
		exit(1);
	}
}

