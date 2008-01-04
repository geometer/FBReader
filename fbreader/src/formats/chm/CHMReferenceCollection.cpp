/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLUnicodeUtil.h>

#include "CHMReferenceCollection.h"
#include "../util/MiscUtil.h"

std::string CHMReferenceCollection::fullReference(const std::string &prefix, std::string reference) {
	reference = MiscUtil::decodeHtmlURL(reference);
	if ((reference.length() > 0) && (reference[0] == '/')) {
		return reference;
	}
	const int index = reference.rfind("::");
	if (index != -1) {
		return reference.substr(index + 2);
	}
	
	int counter = 0;
	while (reference.substr(counter * 3, 3) == "../") {
		++counter;
	}

	int slashIndex = prefix.length() - 1;
	for (int i = 0; (i < counter) && (slashIndex > 0); ++i) {
		slashIndex = prefix.rfind('/', slashIndex - 1);
	}
	return prefix.substr(0, slashIndex + 1) + reference.substr(counter * 3);
}

CHMReferenceCollection::CHMReferenceCollection() : myPrefix("/") {
}

const std::string &CHMReferenceCollection::addReference(const std::string &reference, bool doConvert) {
	if (reference.empty()) {
		return reference;
	}
	std::string fullRef = doConvert ? fullReference(myPrefix, reference) : MiscUtil::decodeHtmlURL(reference);

	const int index = fullRef.find('#');
	if (index == -1) {
		fullRef = ZLUnicodeUtil::toLower(fullRef);
	} else {
		fullRef = ZLUnicodeUtil::toLower(fullRef.substr(0, index));
	}
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
	myPrefix = MiscUtil::decodeHtmlURL(fileName.substr(0, fileName.rfind('/') + 1));
}

const std::string &CHMReferenceCollection::prefix() const {
	return myPrefix;
}
