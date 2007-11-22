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

#include <ZLDir.h>
#include <ZLFile.h>
#include <ZLibrary.h>
#include <ZLUnicodeUtil.h>

#include "ZLLanguageDetector.h"
#include "ZLLanguageMatcher.h"

// 0: no break
// 1: break and skip
// 2: skip word
// 3: skip word, break after
static unsigned char SYMBOL_TYPE[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 1, 1, 1, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 1, 3, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

ZLLanguageDetector::LanguageInfo::LanguageInfo(const std::string &language, const std::string &encoding) : Language(language), Encoding(encoding) {
}

ZLLanguageDetector::ZLLanguageDetector() {
	const std::string dirName =
		ZLibrary::ZLibraryDirectory() + ZLibrary::FileNameDelimiter + "languagePatterns.zip";
	shared_ptr<ZLDir> dir = ZLFile(dirName).directory(false);
	if (!dir.isNull()) {
		std::vector<std::string> fileNames;
		dir->collectFiles(fileNames, false);
		for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
			const int index = it->find('_');
			if (index != -1) {
				const std::string language = it->substr(0, index);
				const std::string encoding = it->substr(index + 1);
				shared_ptr<ZLLanguageMatcher> matcher = new ZLLanguageMatcher(
					dir->itemPath(*it), new LanguageInfo(language, encoding)
				);
				if (encoding == "UTF-8") {
					myUtf8Matchers.push_back(matcher);
				} else if (encoding == "US-ASCII") {
					myUtf8Matchers.push_back(matcher);
					myNonUtf8Matchers.push_back(matcher);
				} else {
					myNonUtf8Matchers.push_back(matcher);
				}
			}
		}
	}
}

ZLLanguageDetector::~ZLLanguageDetector() {
}

shared_ptr<ZLLanguageDetector::LanguageInfo> ZLLanguageDetector::findInfo(const char *buffer, size_t length, int matchingCriterion) {
	const char *start = buffer;
	const char *end = start + length;

	enum { ASCII, UTF8, OTHER } encodingType = ASCII;
	int nonLeadingCharsCounter = 0;
	for (const char *ptr = start; ptr != end; ++ptr) {
		if (nonLeadingCharsCounter == 0) {
			if ((*ptr & 0x80) != 0) {
				encodingType = UTF8;
				if ((*ptr & 0xE0) == 0xC0) {
					nonLeadingCharsCounter = 1;
				} else if ((*ptr & 0xF0) == 0xE0) {
					nonLeadingCharsCounter = 2;
				} else {
					encodingType = OTHER;
					break;
				}
			}
		} else {
			if ((*ptr & 0xC0) != 0x80) {
				encodingType = OTHER;
				break;
			}
			--nonLeadingCharsCounter;
		}
	}
	Vector &matchers = (encodingType == UTF8) ? myUtf8Matchers : myNonUtf8Matchers;

	const char *wordStart = start;
	bool containsSpecialSymbols = false;
	std::string word;
	for (const char *ptr = start; ptr != end; ++ptr) {
		switch (SYMBOL_TYPE[(unsigned char)*ptr]) {
			case 0:
				break;
			case 1:
				if (!containsSpecialSymbols && (ptr > wordStart)) {
					size_t length = ptr - wordStart;
					if (encodingType == UTF8) {
						length = ZLUnicodeUtil::utf8Length(wordStart, length);
					}
					if (length < 5) {
						word.append(wordStart, ptr - wordStart);	
						for (Vector::iterator it = matchers.begin(); it != matchers.end(); ++it) {
							(*it)->processWord(word);
						}
						word.erase();
					}
				}
				wordStart = ptr + 1;
				containsSpecialSymbols = false;
				break;
			case 2:
				containsSpecialSymbols = true;
				break;
			case 3:
				wordStart = ptr + 1;
				containsSpecialSymbols = false;
				break;
		}
	}

	shared_ptr<LanguageInfo> info;
	for (Vector::const_iterator it = matchers.begin(); it != matchers.end(); ++it) {
		int criterion = (*it)->criterion();
		if (criterion > matchingCriterion) {
			info = (*it)->info();
			matchingCriterion = criterion;
		}
		(*it)->reset();
	}

	return info;
}
