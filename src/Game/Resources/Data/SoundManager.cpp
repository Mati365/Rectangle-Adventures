/*
 * SoundsPack.cpp
 *
 *  Created on: 17-08-2013
 *      Author: mateusz
 */
#include "SoundManager.hpp"

/**
 * Motyw dźwięku!
 */
_SoundINFO SoundManager::sounds_assets[DIE_SOUND + 1] {
														{
															JUMP_SOUND,
															"skok.wav",
															MIX_MAX_VOLUME / 2 },
														{
															GUN_SHOT_SOUND,
															"laser.wav",
															MIX_MAX_VOLUME / 8 },
														{
															MENU_CHOOSE_SOUND,
															"wybor_menu.wav",
															MIX_MAX_VOLUME / 2 },
														{
															SCORE_SOUND,
															"punkt.wav",
															MIX_MAX_VOLUME / 4 },
														{
															SPIKES_SOUND,
															"igly.wav",
															MIX_MAX_VOLUME },
														{
															EARTH_QUAKE_SOUND_1,
															"trzesienie_ziemi.wav",
															MIX_MAX_VOLUME },
														{
															EARTH_QUAKE_SOUND_2,
															"trzesienie_ziemi_2.wav",
															MIX_MAX_VOLUME },
														{
															DIE_SOUND,
															"smierc.wav",
															MIX_MAX_VOLUME }, };

/**
 * Wczytywanie paczki dźwięków!
 */
void SoundManager::loadSoundsPack() {
	BEGIN_LOADING("SFX extracting:");
	//
	for (usint i = 0; i < DIE_SOUND + 1; ++i) {
		_SoundINFO& info = sounds_assets[i];
		sounds[info.id] = {Player::getInstance().loadSound(info.path), info.volume};
		//
		PROGRESS_LOADING();
	}
	//
	END_LOADING();
}

/**
 * Odtwarzanie dźwięku po identyfikatorze
 */
void SoundManager::playResourceSound(usint _id) {
	if (sounds.empty()) {
		loadSoundsPack();
	}
	Player::getInstance().playChunk(sounds[_id].chunk, sounds[_id].volume);
}

/**
 * Usuwanie dźwięków
 */
void SoundManager::unloadSoundsPack() {
	for (auto& obj : sounds) {
		if (!obj.second.chunk) {
			continue;
		}
		Player::getInstance().closeChunk(obj.second.chunk);
	}
}

/**
 * Kasowanie dźwięków
 */
SoundManager::~SoundManager() {
	/** Zamykanie Playera */
	Player::getInstance().closeMixAudio();
	
	/** Kasowanie dźwięków */
	unloadSoundsPack();
}
