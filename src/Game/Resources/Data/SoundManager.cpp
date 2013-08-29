/*
 * SoundsPack.cpp
 *
 *  Created on: 17-08-2013
 *      Author: mateusz
 */
#include "SoundManager.hpp"

/**
 * Wczytywanie paczki dźwięków!
 */
void SoundManager::loadSoundsPack() {
	sounds[JUMP_SOUND] = {Player::getInstance().loadSound("skok.wav"), MIX_MAX_VOLUME / 2};
	sounds[GUN_SHOT_SOUND] = {Player::getInstance().loadSound("laser.wav"), MIX_MAX_VOLUME / 8};
	sounds[MENU_CHOOSE_SOUND] = {Player::getInstance().loadSound("wybor_menu.wav"), MIX_MAX_VOLUME / 2};
	sounds[SCORE_SOUND] = {Player::getInstance().loadSound("punkt.wav"), MIX_MAX_VOLUME / 4};
	sounds[SPIKES_SOUND] = {Player::getInstance().loadSound("igly.wav"), MIX_MAX_VOLUME};
	sounds[EARTH_QUAKE_SOUND_1] = {Player::getInstance().loadSound("trzesienie_ziemi.wav"), MIX_MAX_VOLUME};
	sounds[EARTH_QUAKE_SOUND_2] = {Player::getInstance().loadSound("trzesienie_ziemi_2.wav"), MIX_MAX_VOLUME};
	sounds[DIE_SOUND] = {Player::getInstance().loadSound("smierc.wav"), MIX_MAX_VOLUME};
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
