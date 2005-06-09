/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <algorithm>
#include <vector>

#include <abstract/ZLUnicodeUtil.h>
#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>

#include "TeXHyphenator.h"
#include "HyphenationReader.h"

Hyphenator &Hyphenator::instance() {
	if (ourInstance == 0) {
		ourInstance = new TeXHyphenator();
	}
	return *ourInstance;
}

TeXHyphenationPattern::TeXHyphenationPattern(unsigned short *ucs2String, int length) {
	myLength = length;
	mySymbols = ucs2String;
	myValues = NULL;
}

TeXHyphenationPattern::TeXHyphenationPattern(const std::string &utf8String) {
	myLength = 0;

	ZLUnicodeUtil::Ucs2String ucs2String;
	ZLUnicodeUtil::utf8ToUcs2(ucs2String, utf8String);

	const int len = ucs2String.size();
	for (int i = 0; i < len; i++) {
	  if ((ucs2String[i] < '0') || (ucs2String[i] > '9')) {
			myLength++;
		}
	}

	mySymbols = new unsigned short[myLength];
	myValues = new unsigned char[myLength + 1];

	myValues[0] = 0;
	for (int j = 0, k = 0; j < len; j++) {
	  if ((ucs2String[j] >= '0') && (ucs2String[j] <= '9')) {
			myValues[k] = ucs2String[j] - '0';
		} else {
			mySymbols[k] = ucs2String[j];
			k++;
			myValues[k] = 0;
		}
	}
}

TeXHyphenationPattern::~TeXHyphenationPattern() {
	if (myValues != NULL) {
		delete[] mySymbols;
		delete[] myValues;
	}
}

void TeXHyphenationPattern::apply(unsigned char *values) const {
	for (int i = 0; i <= myLength; i++) {
		if (values[i] < myValues[i]) {
			values[i] = myValues[i];
		}
	}
}

bool TeXPatternComparator::operator() (const TeXHyphenationPattern *p1, const TeXHyphenationPattern *p2) const {
	bool firstIsShorter = p1->myLength < p2->myLength;
	int minLength = firstIsShorter ? p1->myLength : p2->myLength;
	unsigned short *symbols1 = p1->mySymbols;
	unsigned short *symbols2 = p2->mySymbols;
	for (int i = 0; i < minLength; i++) {
		if (symbols1[i] < symbols2[i]) {
			return true;
		} else if (symbols1[i] > symbols2[i]) {
			return false;
		}
	}
	return firstIsShorter;
}

static std::vector<unsigned char> values;
static TeXPatternComparator comparator = TeXPatternComparator();

void TeXHyphenator::hyphenate(std::vector<unsigned short> &ucs2String, std::vector<unsigned char> &mask, int length) const {
	if (myPatternTable.empty()) {
		for (int i = 0; i < length - 1; i++) {
			mask[i] = false;
		}
		return;
	}
	
	values.assign(length + 1, 0);
	
	for (int j = 0; j < length - 2; j++) {
		TeXHyphenator::PatternIterator dictionaryPattern = myPatternTable.begin();
		for (int k = 1; k <= length - j; k++) {
			TeXHyphenationPattern pattern(&ucs2String[j], k);
			if (comparator(&pattern, *dictionaryPattern)) {
				continue;
			}
			dictionaryPattern =
				std::lower_bound(myPatternTable.begin(), myPatternTable.end(), &pattern, comparator);
			if (dictionaryPattern == myPatternTable.end()) {
				break;
			}
			if (!comparator(&pattern, *dictionaryPattern)) {
				(*dictionaryPattern)->apply(&values[j]);
			}
		}
	}

	for (int i = 0; i < length - 1; i++) {
		mask[i] = values[i + 1] % 2 == 1;
	}
}

TeXHyphenator::~TeXHyphenator() {
	unload();
}

void TeXHyphenator::load(const std::string &language) {
	if (language == myLanguage) {
		return;
	}
	myLanguage = language;
	
	unload();

	std::string fileName = PatternZip + ":" + language + ".pattern";
	ZLInputStream *stream = ZLFSManager::instance().createInputStream(fileName);
	if (stream != 0) {
		HyphenationReader reader(this);
		reader.readDocument(*stream);
		delete stream;
	}
	
	std::sort(myPatternTable.begin(), myPatternTable.end(), TeXPatternComparator());
}

void TeXHyphenator::unload() {
	for (PatternIterator it = myPatternTable.begin(); it != myPatternTable.end(); it++) {
		delete *it;
	}
	myPatternTable.clear();
}
