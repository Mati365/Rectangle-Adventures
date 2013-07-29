/*
 * Tools.hpp
 *
 *  Created on: 15-04-2013
 *      Author: mateusz
 */

#ifndef TOOLS_HPP_
#define TOOLS_HPP_
#include <cstdlib>
#include <ctime>
#include <deque>
#include <map>
#include <cmath>

using namespace std;

#define USINT_SUB(a, b) ((int)a - (int)b < 0?a=0:a -= b)
#define PI 3.14159265
#define TO_RAD(a) (a * PI / 180)

typedef unsigned short int usint;

template<typename T = int>
T getIntRandom(T min, T max) {
	return rand() % (max - min + 1) + min;
}

/**
 * Prosta implementacja scoped_ptr()
 * z biblioteki boost w razie jej
 * niedostępności na innych platformach
 */
template<class T>
class AllocKiller {
	private:
		T* ptr;
		long* counter; // licznik referencji

	public:
		/**
		 * Przy: void dupa(allocKiller);
		 * void dupa(allocKiller(zmienna));
		 * zamiast
		 * void dupa(zmienna);
		 */
		explicit AllocKiller(T* _ptr) :
				ptr(_ptr),
				counter(NULL) {
			pushReference();
		}

		AllocKiller(const AllocKiller& _copy) :
				ptr(_copy.ptr),
				counter(_copy.counter) {
			pushReference();
		}

		operator T*() {
			return ptr;
		}

		T* operator->() const {
			return ptr;
		}

		T* getPtr() const {
			return ptr;
		}

		~AllocKiller() {
			releaseReference();
		}

	private:
		void pushReference() {
			if (!ptr) {
				return;;
			}
			if (!counter) {
				counter = new long(1);
			} else {
				(*counter)++;
			}
		}

		void releaseReference() {
			(*counter)--;
			if ((*counter) == 0) {
				delete counter;
				delete ptr;
			}
		}
};

/**
 * Migawki stanu gry, przywracanie gry do momentu
 * przed np. śmiercią bądź intrem
 * WSZYSTKO DEKLAROWANE DYNAMICZNIE!!!
 */
namespace Memory {
	class Cloneable {
		public:
			virtual Cloneable* getClone() const = 0;
			virtual bool recover(Cloneable*) = 0;

			virtual ~Cloneable() {
			}
	};

	class Snapshot {
		public:
			class SourcePtr {
				public:
					Cloneable* source;
					Cloneable* clone;

					SourcePtr(Cloneable* _source, Cloneable* _clone) :
							source(_source),
							clone(_clone) {
					}
			};
		private:
			deque<SourcePtr> clones;

		public:
			Snapshot() {
			}

			Snapshot(initializer_list<Cloneable*> _clones) {
				for (auto iter = _clones.begin(); iter != _clones.end();
						++iter) {
					add(*iter);
				}
			}

			deque<SourcePtr>* getClones() {
				return &clones;
			}

			SourcePtr& operator[](usint i) {
				return clones[i];
			}

			bool add(Cloneable* _cloneable) {
				if (!_cloneable) {
					return false;
				}
				clones.push_back(SourcePtr(_cloneable, _cloneable->getClone()));
				return true;
			}

			bool recover();
			virtual ~Snapshot() {
			}
	};

	class SnapshotManager {
		private:
			deque<Snapshot> snapshots;

		public:
			void add(const Snapshot& _snapshot) {
				snapshots.push_back(_snapshot);
			}

			Snapshot& operator[](usint i) {
				return snapshots[i];
			}

			bool recoverLast();
	};
}
#endif /* TOOLS_HPP_ */
