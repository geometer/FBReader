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

#ifndef __TEXHYPHENATOR_H__
#define __TEXHYPHENATOR_H__

#include <vector>
#include <string>

#include "Hyphenator.h"

class TeXHyphenationPattern {

public:
	TeXHyphenationPattern(unsigned short *ucs2String, int length);
	TeXHyphenationPattern(const std::string &utf8String);
	~TeXHyphenationPattern();

	int length() const { return myLength; }
	void apply(unsigned char *values) const;

private:
	int myLength;
	unsigned short *mySymbols;
	unsigned char *myValues;

friend class TeXPatternComparator;
};

class TeXPatternComparator {

public:
	bool operator() (const TeXHyphenationPattern *p1, const TeXHyphenationPattern *p2);
};

class TeXHyphenator : public Hyphenator {

public:
	static std::string PatternZip;

public:
	~TeXHyphenator();

	void load(const std::string &language);
	void unload();

protected:
	void hyphenate(unsigned short *ucs2String, bool *mask, int length) const;

private:
	typedef std::vector<TeXHyphenationPattern*>::const_iterator PatternIterator;

	std::vector<TeXHyphenationPattern*> myPatternTable;
	std::string myLanguage;

friend class HyphenationReader;
};

#endif /* __TEXHYPHENATOR_H__ */
