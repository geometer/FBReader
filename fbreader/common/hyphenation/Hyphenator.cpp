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

#include <abstract/ZLUnicodeUtil.h>

#include "Hyphenator.h"

#include "../textview/Word.h"

Hyphenator *Hyphenator::ourInstance = 0;

HyphenationInfo::HyphenationInfo(int length) {
	myLength = length;
	myMask = new bool[myLength - 1];
}

HyphenationInfo::HyphenationInfo(const HyphenationInfo &info) {
	myLength = info.myLength;
	myMask = new bool[myLength - 1];
	for (int i = 0; i < myLength - 1; i++) {
		myMask[i] = info.myMask[i];
	}
}

HyphenationInfo::~HyphenationInfo() {
	delete[] myMask;
}

bool HyphenationInfo::isHyphenationPossible(int position) {
	return (position >= 0) && (position < myLength - 1) && myMask[position];
}

HyphenationInfo Hyphenator::info(const Word &word) const {
	ZLUnicodeUtil::Ucs2String ucs2Vector;
	ZLUnicodeUtil::utf8ToUcs2(ucs2Vector, word.utf8String());

	int wordLength = word.length();
	unsigned short *pattern = new unsigned short[wordLength + 2];
	pattern[0] = ' ';
	for (unsigned int i = 0; i < ucs2Vector.size(); i++) {
		unsigned short symbol = ucs2Vector[i];
		pattern[i + 1] = ZLUnicodeUtil::isLetter(symbol) ? ZLUnicodeUtil::toLower(symbol) : ' ';
	}
	pattern[wordLength + 1] = ' ';

	HyphenationInfo info(wordLength + 2);
	hyphenate(pattern, info.myMask, wordLength + 2);

	for (int i = 0; i < wordLength + 1; i++) {
		if ((i < 2) || (i > wordLength - 2)) {
			info.myMask[i] = false;
		} else if (ucs2Vector[i - 1] == '-') {
			info.myMask[i] =
				(i >= 3) &&
				ZLUnicodeUtil::isLetter(ucs2Vector[i - 3]) &&
				ZLUnicodeUtil::isLetter(ucs2Vector[i - 2]) &&
				ZLUnicodeUtil::isLetter(ucs2Vector[i]) &&
				ZLUnicodeUtil::isLetter(ucs2Vector[i + 1]);
		} else {
			info.myMask[i] = info.myMask[i] &&
			ZLUnicodeUtil::isLetter(ucs2Vector[i - 2]) &&
			ZLUnicodeUtil::isLetter(ucs2Vector[i - 1]) &&
			ZLUnicodeUtil::isLetter(ucs2Vector[i]) &&
			ZLUnicodeUtil::isLetter(ucs2Vector[i + 1]);
		}
	}

	delete[] pattern;

	return info;
}
