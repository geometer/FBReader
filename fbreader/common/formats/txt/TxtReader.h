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

#ifndef __TXTREADER_H__
#define __TXTREADER_H__

#include <string>

#include <abstract/ZLXMLReader.h>

class ZLInputStream;

class TxtTextConverter : public ZLXMLReader {

protected:
	const Tag *tags() const;

public:
	TxtTextConverter(const char *encoding) FORMATS_SECTION;
	~TxtTextConverter() FORMATS_SECTION;
	void convertBuffer(std::vector<std::string> &buffer) FORMATS_SECTION;
	void convertString(std::string &string) FORMATS_SECTION;
	
public:
	void startElementHandler(int tag, const char **attributes) FORMATS_SECTION;
	void endElementHandler(int tag) FORMATS_SECTION;
	void characterDataHandler(const char *text, int len) FORMATS_SECTION;

private:
	std::vector<std::string> *myBuffer;
};

class TxtReader {

public:
	void readDocument(ZLInputStream &stream, const std::string &encoding) FORMATS_SECTION;

protected:
	TxtReader() FORMATS_SECTION;
	virtual ~TxtReader() FORMATS_SECTION;

protected:
	virtual void startDocumentHandler() FORMATS_SECTION = 0;
	virtual void endDocumentHandler() FORMATS_SECTION = 0;

	virtual bool characterDataHandler(const char *text, int len) FORMATS_SECTION = 0;
	virtual bool newLineHandler() FORMATS_SECTION = 0;

protected:
	TxtTextConverter *myConverter;
};

inline TxtTextConverter::TxtTextConverter(const char *encoding) : ZLXMLReader(encoding) {}
inline TxtTextConverter::~TxtTextConverter() {}

#endif /* __TXTREADER_H__ */
