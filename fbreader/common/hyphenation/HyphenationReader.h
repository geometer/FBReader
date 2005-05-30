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

#ifndef __HYPHENATIONREADER_H__
#define __HYPHENATIONREADER_H__

#include <string>

#include <abstract/ZLXMLReader.h>

class TeXHyphenator;

class HyphenationReader : public ZLXMLReader {

public:
	HyphenationReader(TeXHyphenator *hyphenator) HYPHENATION_SECTION;
	~HyphenationReader() HYPHENATION_SECTION;

	const Tag *tags() const HYPHENATION_SECTION;

	void startElementHandler(int tag, const char **attributes) HYPHENATION_SECTION;
	void endElementHandler(int tag) HYPHENATION_SECTION;
	void characterDataHandler(const char *text, int len) HYPHENATION_SECTION;

private:
	TeXHyphenator *myHyphenator;
	bool myReadPattern;
	std::string myBuffer;
};

#endif /* __HYPHENATIONREADER_H__ */
