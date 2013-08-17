/*
 * Sounds.hpp
 *
 *  Created on: 08-08-2013
 *      Author: mateusz
 */

#ifndef SOUNDS_HPP_
#define SOUNDS_HPP_
#include <SDL/SDL_mixer.h>

#include "../../Resources/Data/Resources.hpp"

namespace Sound {
	/**
	 * Odtwarzacz plików wav
	 */
	class wavPlayer {
		private:
			int audio_rate;
			Uint16 audio_format;
			int audio_channels;
			int audio_buffers;

			/**
			 * Wzorzec singleton!
			 */
			wavPlayer();

		public:
			/**
			 * Wczytywanie pliku wav z systemu plików
			 */
			Mix_Chunk* loadSound(const char*);

			/**
			 * Odtwarzanie pliku wav
			 */
			void playChunk(Mix_Chunk*, float);

			/**
			 * Zamykanie pliku
			 */
			void closeChunk(Mix_Chunk*);

			static wavPlayer& getInstance() {
				static wavPlayer player;
				//
				return player;
			}

			/**
			 * Zamykanie odtwarzacza
			 */
			void closeMixAudio() {
				Mix_CloseAudio();
			}

		protected:
			/**
			 * Otwieranie mix
			 */
			void openMixAudio();
	};
}

#endif /* SOUNDS_HPP_ */
