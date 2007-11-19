/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLFile.h>
#include <ZLInputStream.h>

#include "ZLLanguageMatcher.h"

ZLLanguageMatcher::ZLLanguageMatcher(const std::string &fileName, shared_ptr<ZLLanguageDetector::LanguageInfo> info) : myInfo(info) {
	shared_ptr<ZLInputStream> dictionaryStream = ZLFile(fileName).inputStream();
	if (dictionaryStream.isNull() || !dictionaryStream->open()) {
		return;
	}
	
	const size_t BUFFER_SIZE = 20480;
	char *buffer = new char[BUFFER_SIZE];
	const char *start = buffer;
	const char *end = start + dictionaryStream->read(buffer, BUFFER_SIZE);
	dictionaryStream->close();
	const char *wordStart = start;
	for (const char *ptr = start; ptr != end; ++ptr) {
		if (*ptr == '\n') {
			myDictionary.insert(std::string(wordStart, ptr - wordStart));	
			wordStart = ptr + 1;
		}
	}
	delete[] buffer;

	reset();
}

void ZLLanguageMatcher::reset() {
	myProCounter = 0;
	myContraCounter = 0;
}

void ZLLanguageMatcher::processWord(const std::string &word) {
	if (myDictionary.find(word) != myDictionary.end()) {
		++myProCounter;
	} else {
		++myContraCounter;
	}
}

int ZLLanguageMatcher::matchingCriteria() const {
	if ((myProCounter == 0) && (myContraCounter == 0)) {
		return -1000;
	}
	return (myProCounter * 2000) / (myProCounter + myContraCounter) - 1000;
}

shared_ptr<ZLLanguageDetector::LanguageInfo> ZLLanguageMatcher::info() const {
	return myInfo;
}
