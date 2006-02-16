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

#ifndef __TEXHYPHENATOR_H__
#define __TEXHYPHENATOR_H__

#include <vector>
#include <string>

#include "Hyphenator.h"

class TeXHyphenationPattern {

public:
	TeXHyphenationPattern(unsigned short *ucs2String, int length) HYPHENATION_SECTION;
	TeXHyphenationPattern(const std::string &utf8String) HYPHENATION_SECTION;
	~TeXHyphenationPattern() HYPHENATION_SECTION;

	int length() const { return myLength; }
	void apply(unsigned char *values) const HYPHENATION_SECTION;

private:
	int myLength;
	unsigned short *mySymbols;
	unsigned char *myValues;

friend class TeXPatternComparator;
};

class TeXPatternComparator {

public:
	bool operator() (const TeXHyphenationPattern *p1, const TeXHyphenationPattern *p2) const HYPHENATION_SECTION;
};

class TeXHyphenator : public Hyphenator {

private:
	static const std::string PatternZip;
	static std::vector<std::string> LanguageCodes;
	static std::vector<std::string> LanguageNames;
	static void collectLanguages();

public:
	static const std::vector<std::string> &languageCodes();
	static const std::vector<std::string> &languageNames();

public:
	TeXHyphenator() {}
	~TeXHyphenator() HYPHENATION_SECTION;

	void load(const std::string &language) HYPHENATION_SECTION;
	void unload() HYPHENATION_SECTION;

protected:
	void hyphenate(std::vector<unsigned short> &ucs2String, std::vector<unsigned char> &mask, int length) const HYPHENATION_SECTION;

private:
	typedef std::vector<TeXHyphenationPattern*>::const_iterator PatternIterator;

	std::vector<TeXHyphenationPattern*> myPatternTable;
	std::string myLanguage;

friend class HyphenationReader;
};

#endif /* __TEXHYPHENATOR_H__ */
