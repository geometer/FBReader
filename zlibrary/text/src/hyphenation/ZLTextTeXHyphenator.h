/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLTEXTTEXHYPHENATOR_H__
#define __ZLTEXTTEXHYPHENATOR_H__

#include <vector>
#include <string>

#include "ZLTextHyphenator.h"

class ZLTextTeXHyphenationPattern {

public:
	ZLTextTeXHyphenationPattern(unsigned short *ucs2String, int length);
	ZLTextTeXHyphenationPattern(const std::string &utf8String);
	~ZLTextTeXHyphenationPattern();

	int length() const { return myLength; }
	void apply(unsigned char *values) const;

private:
	int myLength;
	unsigned short *mySymbols;
	unsigned char *myValues;

friend class ZLTextTeXPatternComparator;
};

class ZLTextTeXPatternComparator {

public:
	bool operator() (const ZLTextTeXHyphenationPattern *p1, const ZLTextTeXHyphenationPattern *p2) const;
};

class ZLTextTeXHyphenator : public ZLTextHyphenator {

private:
	static const std::string PatternZip();

public:
	~ZLTextTeXHyphenator();

	void load(const std::string &language);
	void unload();
	const std::string &language() const;

protected:
	void hyphenate(std::vector<unsigned short> &ucs2String, std::vector<unsigned char> &mask, int length) const;

private:
	typedef std::vector<ZLTextTeXHyphenationPattern*>::const_iterator PatternIterator;

	std::vector<ZLTextTeXHyphenationPattern*> myPatternTable;
	std::string myLanguage;

friend class ZLTextHyphenationReader;
};

#endif /* __ZLTEXTTEXHYPHENATOR_H__ */
