/*
 * Copyright (C) 2007-2010 Geometer Plus <contact@geometerplus.com>
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
#include <ZLDir.h>
#include <ZLUnicodeUtil.h>

#include "ZLLanguageList.h"
#include "ZLLanguageDetector.h"
#include "ZLLanguageMatcher.h"
#include "ZLStatisticsGenerator.h"
#include "ZLStatistics.h"
#include "ZLCharSequence.h"

ZLLanguageDetector::LanguageInfo::LanguageInfo(const std::string &language, const std::string &encoding) : Language(language), Encoding(encoding) {
}

ZLLanguageDetector::ZLLanguageDetector() {
	const ZLFile patternsArchive(ZLLanguageList::patternsDirectoryPath());
	shared_ptr<ZLInputStream> lock = patternsArchive.inputStream();
	shared_ptr<ZLDir> dir = patternsArchive.directory(false);
	if (!dir.isNull()) {
		std::vector<std::string> fileNames;
		dir->collectFiles(fileNames, false);
		for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
			const int index = it->find('_');
			if (index != -1) {
				const std::string language = it->substr(0, index);
				const std::string encoding = it->substr(index + 1);
				shared_ptr<ZLStatisticsBasedMatcher> matcher = new ZLStatisticsBasedMatcher(dir->itemPath(*it), new LanguageInfo(language, encoding));
				myMatchers.push_back(matcher);
			}
		}
	}
}

ZLLanguageDetector::~ZLLanguageDetector() {
}

const unsigned char ZLLanguageDetector::utf8Length[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x00-0x0f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x10-0x1f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x20-0x2f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x30-0x3f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x40-0x4f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x50-0x5f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x60-0x6f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x70-0x7f
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, //0x80-0x8f
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, //0x90-0x9f
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, //0xa0-0xaf
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, //0xb0-0xbf
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, //0xc0-0xcf  c0-c1: overlong encoding: start of a 2-byte sequence, but code point <= 127
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, //0xd0-0xdf
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, //0xe0-0xef
    4,4,4,4,4,8,8,8,8,8,8,8,8,8,8,8  //0xf0-0xff
};

typedef unsigned char uchar;

std::string ZLLanguageDetector::detectUtf8 (const unsigned char *s, int maxLen, bool zeroInvalid) {
	if (!s) return std::string();
	if (maxLen < 1) return std::string();
	uchar ch = 0;
	const uchar *tmpS = s;
	bool isAscii = true;
	while (maxLen > 0) {
		ch = *tmpS++; maxLen--;
		if (!ch) {
			if (maxLen != 0 && zeroInvalid) return std::string();
			break;
		}
		// ascii or utf-8
		uchar t = utf8Length[ch];
		if (t & 0x08) return std::string(); // invalid utf-8 sequence
		if (t) {
			// utf-8
			isAscii = false;
			if (maxLen < --t) return std::string(); // invalid utf-8 sequence
			while (t--) {
				uchar b = *tmpS++; maxLen--;
				if (utf8Length[b] != 9) return std::string(); // invalid utf-8 sequence
			}
		}
	}
	return isAscii ? "ASCII" : "UTF-8";
}


shared_ptr<ZLLanguageDetector::LanguageInfo> ZLLanguageDetector::findInfo(const char *buffer, size_t length, int matchingCriterion) {
	shared_ptr<LanguageInfo> info;
	std::map<int,shared_ptr<ZLMapBasedStatistics> > statisticsMap;
	std::string ucs2;
	if (uchar(buffer[0]) == 0xFE
	        && uchar(buffer[1]) == 0xFF) {
		ucs2 = "UTF-16BE";
	} else if (uchar(buffer[0]) == 0xFF
	           && uchar(buffer[1]) == 0xFE) {
		ucs2 = "UTF-16";
	} else {
		ucs2 = detectUtf8(reinterpret_cast<const uchar *>(buffer), length, true);
	}
	for (SBVector::const_iterator it = myMatchers.begin(); it != myMatchers.end(); ++it) {
		const std::string &encoding = (*it)->info()->Encoding;
		if ((ucs2.empty() && encoding.compare(0, 3, "UTF") && encoding != "ASCII")
		        || (!ucs2.empty() && encoding == ucs2)) {
			const int charSequenceLength = (*it)->charSequenceLength();
			shared_ptr<ZLMapBasedStatistics> &stat = statisticsMap[charSequenceLength];
			if (stat.isNull()) {
				stat = new ZLMapBasedStatistics();
				ZLStatisticsGenerator("\r\n ").generate(
					buffer, length, charSequenceLength, *stat
				);
			}
			const int criterion = (*it)->criterion(*stat);
			if (criterion > matchingCriterion) {
				info = (*it)->info();
				matchingCriterion = criterion;
			}
		}
	}
	return info;
}
