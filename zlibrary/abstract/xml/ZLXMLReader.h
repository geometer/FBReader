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

#ifndef __ZLXMLREADER_H__
#define __ZLXMLREADER_H__

#include <string>
#include <vector>

#include <expat.h>

class ZLInputStream;

class ZLXMLReader {

//protected:
public:
	struct Tag {
		const char *tagName;
		int tagCode;
	};

protected:
	ZLXMLReader();
	virtual ~ZLXMLReader();

	virtual const Tag *tags() const = 0;

public:
	void readDocument(ZLInputStream &stream);

	int tag(const char *name);

	virtual void startElementHandler(int tag, const char **attributes) = 0;
	virtual void endElementHandler(int tag) = 0;
	virtual void characterDataHandler(const char *text, int len) = 0;

protected:
	virtual const std::vector<std::string> &externalDTDs() const;

protected:
	bool myDoBreak;

private:
	XML_Parser myParser;
	char *myParserBuffer;
};

#endif /* __ZLXMLREADER_H__ */
