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

#include "HyphenationReader.h"
#include "TeXHyphenator.h"

enum TagCode {
	_PATTERN,
	_UNKNOWN
};

static const ZLXMLReader::Tag TAGS[] = {
	{ "pattern", _PATTERN },
	{ 0, _UNKNOWN }
};

const ZLXMLReader::Tag *HyphenationReader::tags() const {
	return TAGS;
}

void HyphenationReader::characterDataHandler(const char *text, int len) {
	if (myReadPattern) {
		myBuffer.append(text, len);
	}
}

void HyphenationReader::startElementHandler(int tag, const char **) {
	if (tag == _PATTERN) {
		myReadPattern = true;
	}
}

void HyphenationReader::endElementHandler(int tag) {
	if (tag == _PATTERN) {
		myReadPattern = false;
		if (!myBuffer.empty()) {
			myHyphenator->myPatternTable.push_back(new TeXHyphenationPattern(myBuffer));
		}
		myBuffer.erase();
	}
}

HyphenationReader::HyphenationReader(TeXHyphenator *hyphenator) {
	myHyphenator = hyphenator;
	myReadPattern = false;
}

HyphenationReader::~HyphenationReader() {
}
