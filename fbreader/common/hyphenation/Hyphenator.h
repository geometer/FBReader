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

#ifndef __HYPHENATOR_H__
#define __HYPHENATOR_H__

#include <string>

class Word;

class HyphenationInfo {

private:
	HyphenationInfo(int length) HYPHENATION_SECTION;
	const HyphenationInfo &operator = (HyphenationInfo &info) HYPHENATION_SECTION;

public:
	~HyphenationInfo() {}
	bool isHyphenationPossible(size_t position) HYPHENATION_SECTION;
	
private:
	std::vector<unsigned char> myMask;

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

	virtual bool useBreakingAlgorithm() const HYPHENATION_SECTION = 0;

protected:
	virtual void hyphenate(std::vector<unsigned short> &ucs2String, std::vector<unsigned char> &mask, int length) const HYPHENATION_SECTION = 0;

protected:
	static Hyphenator *ourInstance;
};

inline HyphenationInfo::HyphenationInfo(int length) : myMask(length - 1, false) {
}

inline bool HyphenationInfo::isHyphenationPossible(size_t position) {
	return (position < myMask.size()) && myMask[position];
}

#endif /* __HYPHENATOR_H__ */
