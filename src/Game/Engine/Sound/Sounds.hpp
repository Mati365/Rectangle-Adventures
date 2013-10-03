/*
 * Sounds.hpp
 *
 *  Created on: 08-08-2013
 *      Author: mateusz
 */

#ifndef SOUNDS_HPP_
#define SOUNDS_HPP_
#include <SFML/Audio.hpp>

#include "../../Resources/Data/Resources.hpp"

namespace Sound {
	/** Klasa dzwieku */
	struct _Sound {
			sf::SoundBuffer* buffer;
			float volume;
	};
	
	/** Odtwarzacz plikow wav */
	class Player {
		private:
			/** Wzorzec singleton! */
			Player() {
			}

		public:
			/** Wczytywanie dzwieku */
			sf::SoundBuffer* loadSound(const char*);

			/** Odtwarzanie */
			sf::Sound* generateBuffer(sf::SoundBuffer*, float, bool = false);

			static Player& getInstance() {
				static Player player;
				//
				return player;
			}
	};
}

#endif /* SOUNDS_HPP_ */
