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

#ifndef __HYPHENATOR_H__
#define __HYPHENATOR_H__

#include <string>

class Word;

class HyphenationInfo {

private:
	HyphenationInfo(int length) HYPHENATION_SECTION;
	const HyphenationInfo &operator = (HyphenationInfo &info) HYPHENATION_SECTION;

public:
	HyphenationInfo(const HyphenationInfo &info) HYPHENATION_SECTION;
	~HyphenationInfo() HYPHENATION_SECTION;
	bool isHyphenationPossible(int position) HYPHENATION_SECTION;
	
private:
	int myLength;
	bool *myMask;

friend class Hyphenator;
};

class Hyphenator {

public:
	static Hyphenator &instance() HYPHENATION_SECTION;
	static void deleteInstance() HYPHENATION_SECTION;
	
protected:
	Hyphenator() {}

public:
	virtual ~Hyphenator() {}

	virtual void load(const std::string &language) HYPHENATION_SECTION = 0;
	virtual void unload() HYPHENATION_SECTION = 0;

	HyphenationInfo info(const Word &word) const HYPHENATION_SECTION;

protected:
	virtual void hyphenate(unsigned short *ucs2String, bool *mask, int length) const HYPHENATION_SECTION = 0;

protected:
	static Hyphenator *ourInstance;
};

#endif /* __HYPHENATOR_H__ */
