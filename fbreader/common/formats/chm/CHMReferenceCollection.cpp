/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "CHMReferenceCollection.h"

std::string CHMReferenceCollection::fullReference(const std::string &prefix, const std::string &reference) {
	const int index = reference.rfind("::");
	return (index == -1) ? prefix + reference : reference.substr(index + 2);
}

CHMReferenceCollection::CHMReferenceCollection() : myPrefix("/") {
}

const std::string &CHMReferenceCollection::addReference(const std::string &reference, bool doConvert) {
	if (reference.empty()) {
		return reference;
	}
	std::string fullRef = doConvert ? fullReference(myPrefix, reference) : reference;
	std::set<std::string>::const_iterator it = myReferences.find(fullRef);
	if (it != myReferences.end()) {
		return *it;
	}
	myReferences.insert(fullRef);
	myReferenceQueue.push(fullRef);
	return myReferenceQueue.back();
}

bool CHMReferenceCollection::containsNonProcessedReferences() const {
	return !myReferenceQueue.empty();
}

const std::string CHMReferenceCollection::nextReference() {
	if (myReferenceQueue.empty()) {
		return "";
	}
	const std::string front = myReferenceQueue.front();
	myReferenceQueue.pop();
	return front;
}

void CHMReferenceCollection::setPrefix(const std::string &fileName) {
	myPrefix = fileName.substr(0, fileName.rfind('/') + 1);
}

const std::string &CHMReferenceCollection::prefix() const {
	return myPrefix;
}
