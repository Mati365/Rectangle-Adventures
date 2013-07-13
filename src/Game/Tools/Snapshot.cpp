/*
 * Snapshot.cpp
 *
 *  Created on: 08-05-2013
 *      Author: mateusz
 */
#include "Tools.hpp"

using namespace Memory;

bool Snapshot::recover() {
	for (usint i = 0; i < clones.size(); ++i) {
		clones[i].source->recover(clones[i].clone);
		delete clones[i].clone;
	}
	return true;
}

// ---------------------- Manager --------------

bool SnapshotManager::recoverLast() {
	if (snapshots.empty()) {
		return false;
	}
	snapshots.back().recover();
	snapshots.pop_back();
	return true;
}
