/*
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

#ifndef __ZLXMLREADERINTERNAL_H__
#define __ZLXMLREADERINTERNAL_H__

#include <string>

class ZLXMLReader;

enum XMLParseState {
	XML_PARSE_DATA,
	XML_PARSE_TAG,
};

class ZLXMLReaderInternal {

public:
	ZLXMLReaderInternal(ZLXMLReader &reader, const char *encoding) XML_SECTION;
	~ZLXMLReaderInternal() XML_SECTION;
	void init() XML_SECTION;
	bool parseBuffer(const char *buffer, size_t) XML_SECTION;

private:
	ZLXMLReader &myReader;
	std::string myTagContents;
	XMLParseState myState;
};

#endif /* __ZLXMLREADERINTERNAL_H__ */
