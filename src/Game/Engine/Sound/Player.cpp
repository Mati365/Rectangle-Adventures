/*
 * Player.cpp
 *
 *  Created on: 08-08-2013
 *      Author: mateusz
 */
#include "Sounds.hpp"

using namespace Sound;

/**
 * Wzorzec singleton!
 */
wavPlayer::wavPlayer() :
				audio_rate(22050),
				audio_format(AUDIO_S16SYS),
				audio_channels(2),
				audio_buffers(1024) {
	openMixAudio();
}

/**
 * Wczytywanie pliku wav z systemu plików
 */
Mix_Chunk* wavPlayer::loadSound(const char* label) {
	size_t filesize;
	char* buffer = NULL;
	//
	main_filesystem.getExternalFile(label, &filesize);
	buffer = main_filesystem.getExternalFileContent(label);

	if (!buffer) {
		return NULL;
	}

	SDL_RWops* rw = SDL_RWFromMem(buffer, filesize);
	Mix_Chunk* sound = Mix_LoadWAV_RW(rw, 1);

	delete[] buffer;
	//
	return sound;
}

/**
 * Odtwarzanie pliku wav
 */
void wavPlayer::playChunk(Mix_Chunk* sound) {
	int channel = Mix_PlayChannel(-1, sound, 0);
	if (channel == -1) {
		fprintf(stderr, "Nie mogę odtworzyć dźwięku: %s\n", Mix_GetError());
	}
}

/**
 * Zamykanie pliku
 */
void wavPlayer::closeChunk(Mix_Chunk* sound) {
	Mix_FreeChunk(sound);
}

/**
 * Otwieranie mix
 */
void wavPlayer::openMixAudio() {
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)
			!= 0) {
		fprintf(stderr, "Nie mogę zainicjować dźwięku: %s\n", Mix_GetError());
		exit(1);
	}
}

