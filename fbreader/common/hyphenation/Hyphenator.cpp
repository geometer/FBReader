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
	int wordLength = word.length();
	unsigned short *ucs2String = new unsigned short[wordLength + 2];
	ucs2String[0] = ' ';
	for (int i = 0; i < wordLength; i++) {
		unsigned short symbol = word.charAt(i);
		ucs2String[i + 1] = ZLUnicodeUtil::isLetter(symbol) ? ZLUnicodeUtil::toLower(symbol) : ' ';
	}
	ucs2String[wordLength + 1] = ' ';

	HyphenationInfo info(wordLength + 2);
	hyphenate(ucs2String, info.myMask, wordLength + 2);

	for (int i = 0; i < wordLength + 1; i++) {
		if ((i < 2) || (i > wordLength - 2)) {
			info.myMask[i] = false;
		} else if (word.charAt(i - 1) == '-') {
			info.myMask[i] =
				(i >= 3) &&
				ZLUnicodeUtil::isLetter(word.charAt(i - 3)) &&
				ZLUnicodeUtil::isLetter(word.charAt(i - 2)) &&
				ZLUnicodeUtil::isLetter(word.charAt(i)) &&
				ZLUnicodeUtil::isLetter(word.charAt(i + 1));
		} else {
			info.myMask[i] = info.myMask[i] &&
			ZLUnicodeUtil::isLetter(word.charAt(i - 2)) &&
			ZLUnicodeUtil::isLetter(word.charAt(i - 1)) &&
			ZLUnicodeUtil::isLetter(word.charAt(i)) &&
			ZLUnicodeUtil::isLetter(word.charAt(i + 1));
		}
	}

	delete[] ucs2String;

	return info;
}
