/*
 * SoundsPack.cpp
 *
 *  Created on: 17-08-2013
 *      Author: mateusz
 */
#include "Resources.hpp"

#include "../../Engine/Sound/Sounds.hpp"

using namespace Sound;

_Sound sounds[DIE_SOUND + 1];

/**
 * Wczytywanie paczki dźwięków!
 */
void loadSoundsPack() {
	sounds[JUMP_SOUND] = {wavPlayer::getInstance().loadSound("skok.wav"), MIX_MAX_VOLUME / 2};
	sounds[GUN_SHOT_SOUND] = {wavPlayer::getInstance().loadSound("laser.wav"), MIX_MAX_VOLUME / 8};
	sounds[MENU_CHOOSE_SOUND] = {wavPlayer::getInstance().loadSound("wybor_menu.wav"), MIX_MAX_VOLUME / 2};
	sounds[SCORE_SOUND] = {wavPlayer::getInstance().loadSound("punkt.wav"), MIX_MAX_VOLUME / 4};
	sounds[SPIKES_SOUND] = {wavPlayer::getInstance().loadSound("igly.wav"), MIX_MAX_VOLUME};
	sounds[EARTH_QUAKE_SOUND_1] = {wavPlayer::getInstance().loadSound("trzesienie_ziemi.wav"), MIX_MAX_VOLUME};
	sounds[EARTH_QUAKE_SOUND_2] = {wavPlayer::getInstance().loadSound("trzesienie_ziemi_2.wav"), MIX_MAX_VOLUME};
	sounds[DIE_SOUND] = {wavPlayer::getInstance().loadSound("smierc.wav"), MIX_MAX_VOLUME};
}

/**
 * Odtwarzanie dźwięku po identyfikatorze
 */
void playResourceSound(usint _id) {
	wavPlayer::getInstance().playChunk(sounds[_id].chunk, sounds[_id].volume);
}

void unloadSoundsPack() {
	for (auto& obj : sounds) {
		if (!obj.chunk) {
			continue;
		}
		wavPlayer::getInstance().closeChunk(obj.chunk);
	}
}

