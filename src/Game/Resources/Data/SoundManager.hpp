/*
 * SoundsPack.hpp
 *
 *  Created on: 29-08-2013
 *      Author: mateusz
 */

#ifndef SOUNDMANAGER_HPP_
#define SOUNDMANAGER_HPP_
#include "../../Engine/Sound/Sounds.hpp"

using namespace Sound;

/** Informacje o dźwięku w grze */
struct _SoundINFO {
		usint id;
		const char* path;
		float volume;
};

class SoundManager {
	public:
		/** Dźwięki w grze */
		enum Sounds {
			JUMP_SOUND,
			GUN_SHOT_SOUND,
			MENU_CHOOSE_SOUND,
			SCORE_SOUND,
			SPIKES_SOUND,
			EARTH_QUAKE_SOUND_1, // trzęsienie ziemi
			EARTH_QUAKE_SOUND_2, // trzęsienie ziemi
			DIE_SOUND
		};

		/** Informacje o zasobach */
		static _SoundINFO sounds_assets[];

		map<usint, _Sound> sounds;

	private:
		/** Wczytywanie */
		void loadSoundsPack();

		/** Usuwanie */
		void unloadSoundsPack();

	public:
		/** Singleton */
		static SoundManager& getInstance() {
			static SoundManager manager;
			//
			return manager;
		}

		/**
		 * Odtwarzanie dźwięku z ID
		 * + Wczytywanie paczki dźwięków jeśli
		 * sound jest pusty!
		 */
		void playResourceSound(usint);

		/** Zwalnianie Playera */
		~SoundManager();
};

#endif /* SOUNDSPACK_HPP_ */
