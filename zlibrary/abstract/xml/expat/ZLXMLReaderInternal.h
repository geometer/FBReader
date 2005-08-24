/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLXMLREADERINTERNAL_H__
#define __ZLXMLREADERINTERNAL_H__

#include <expat.h>

class ZLXMLReader;

class ZLXMLReaderInternal {

public:
	ZLXMLReaderInternal(ZLXMLReader &reader, const char *encoding);
	~ZLXMLReaderInternal();
	void init();
	bool parseBuffer(const char *buffer, size_t len);

private:
	ZLXMLReader &myReader;
	XML_Parser myParser;
	bool myInitialized;
};

#endif /* __ZLXMLREADERINTERNAL_H__ */
