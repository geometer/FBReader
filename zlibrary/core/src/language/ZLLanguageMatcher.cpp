/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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
#include <ZLUnicodeUtil.h>

#include "ZLLanguageMatcher.h"

const std::string ZLLanguageMatcher::UTF8_ENCODING_NAME = "UTF-8";

ZLLanguageMatcher::ZLLanguageMatcher(shared_ptr<ZLLanguageDetector::LanguageInfo> info) : myInfo(info) {
}

ZLLanguageMatcher::~ZLLanguageMatcher() {
}

shared_ptr<ZLLanguageDetector::LanguageInfo> ZLLanguageMatcher::info() const {
	return myInfo;
}

ZLWordBasedMatcher::ZLWordBasedMatcher(shared_ptr<ZLLanguageDetector::LanguageInfo> info) :ZLLanguageMatcher(info) {
}

ZLLanguagePatternBasedMatcher::ZLLanguagePatternBasedMatcher(const std::string &fileName, shared_ptr<ZLLanguageDetector::LanguageInfo> info) : ZLWordBasedMatcher(info) {
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

void ZLLanguagePatternBasedMatcher::reset() {
	myProCounter = 1;
	myContraCounter = 1;
}

void ZLLanguagePatternBasedMatcher::processWord(const std::string &word, int length) {
	if (length < 5) {
		if (myDictionary.find(word) != myDictionary.end()) {
			++myProCounter;
		} else {
			++myContraCounter;
		}
	}
}

int ZLLanguagePatternBasedMatcher::criterion() const {
	return myProCounter * 2000 / (myProCounter + myContraCounter) - 1000;
}

ZLChineseMatcher::ZLChineseMatcher(const std::string &encoding) : ZLLanguageMatcher(new ZLLanguageDetector::LanguageInfo("zh", encoding)) {
	reset();
}

void ZLChineseMatcher::reset() {
	myChineseCharacterCounter = 1;
	myNonChineseCharacterCounter = 1;
}

int ZLChineseMatcher::criterion() const {
	return (myChineseCharacterCounter * 2000) / (myChineseCharacterCounter + myNonChineseCharacterCounter) - 1000;
}

ZLChineseUtf8Matcher::ZLChineseUtf8Matcher() : ZLWordBasedMatcher(new ZLLanguageDetector::LanguageInfo("zh", UTF8_ENCODING_NAME)) {
	reset();
}

void ZLChineseUtf8Matcher::reset() {
	myChineseCharacterCounter = 1;
	myNonChineseCharacterCounter = 1;
}

int ZLChineseUtf8Matcher::criterion() const {
	return (myChineseCharacterCounter * 2000) / (myChineseCharacterCounter + myNonChineseCharacterCounter) - 1000;
}

void ZLChineseUtf8Matcher::processWord(const std::string &word, int length) {
	static bool ignorableSymbols[256] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // ASCII symbols
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, // 0xA2, 0xA3, 0xA5, 0xA7
		1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, // 0xB0, 0xB1, 0xB7
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, // 0xD7
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0  // 0xF7
	};

	const char *ptr = word.data();
	ZLUnicodeUtil::Ucs2Char symbol;
	for (int i = 0; i < length; ++i) {
		ptr += ZLUnicodeUtil::firstChar(symbol, ptr);
		if (symbol <= 0xFF) {
			if (!ignorableSymbols[symbol]) {
				++myNonChineseCharacterCounter;
			}
		} else if (symbol >= 0x2000) {
			++myChineseCharacterCounter;
		} else {
			++myNonChineseCharacterCounter;
		}
	}
}

ZLChineseBig5Matcher::ZLChineseBig5Matcher() : ZLChineseMatcher("Big5") {
}

void ZLChineseBig5Matcher::processBuffer(const unsigned char *ptr, const unsigned char *end) {
	for (; ptr < end; ++ptr) {
		if (*ptr <= 0x7F) {
			continue;
		}
		if ((*ptr <= 0xA0) || (*ptr >= 0xFA)) {
			++ptr;
			++myNonChineseCharacterCounter;
		} else {
			unsigned char firstChar = *ptr;
			++ptr;
			if (ptr >= end) {
				myNonChineseCharacterCounter += 10;
			} else if (*ptr < 0x40) {
				myNonChineseCharacterCounter += 10;
			} else if ((0xC6 <= firstChar) && (firstChar <= 0xC8) && (*ptr >= 0xA1)) {
				++myNonChineseCharacterCounter;
			} else {
				++myChineseCharacterCounter;
			}
		}
	}
}

ZLChineseGBKMatcher::ZLChineseGBKMatcher() : ZLChineseMatcher("GBK") {
}

void ZLChineseGBKMatcher::processBuffer(const unsigned char *ptr, const unsigned char *end) {
	for (; ptr < end; ++ptr) {
		if (*ptr <= 0x7F) {
			continue;
		}
		if ((*ptr == 0x80) || (*ptr == 0xFF)) {
			++ptr;
			myNonChineseCharacterCounter += 10;
		} else {
			unsigned char firstChar = *ptr;
			++ptr;
			if (ptr >= end) {
				myNonChineseCharacterCounter += 10;
			} else if ((*ptr < 0x40) || (*ptr == 0x7F) || (*ptr == 0xFF)) {
				myNonChineseCharacterCounter += 10;
			} else if (((0xAA <= firstChar) && (firstChar <= 0xAF) && (*ptr >= 0xA1)) ||
								 ((0xF8 <= firstChar) && (firstChar <= 0xFE) && (*ptr >= 0xA1)) ||
								 ((0xA1 <= firstChar) && (firstChar <= 0xA7) && (*ptr <= 0xA0))) {
				++myNonChineseCharacterCounter;
			} else {
				++myChineseCharacterCounter;
			}
		}
	}
}
