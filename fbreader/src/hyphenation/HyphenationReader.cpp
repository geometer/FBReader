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

#include "HyphenationReader.h"
#include "TeXHyphenator.h"

void HyphenationReader::characterDataHandler(const char *text, int len) {
	if (myReadPattern) {
		myBuffer.append(text, len);
	}
}

static const std::string PATTERN = "pattern";
static const std::string LINE_BREAKING_ALGORITHM = "lineBreakingAlgorithm";

void HyphenationReader::startElementHandler(const char *tag, const char **) {
	if (PATTERN == tag) {
		myReadPattern = true;
	} else if (LINE_BREAKING_ALGORITHM == tag) {
		myHyphenator->myUseBreakingAlgorithm = true;
	}
}

void HyphenationReader::endElementHandler(const char *tag) {
	if (PATTERN == tag) {
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
