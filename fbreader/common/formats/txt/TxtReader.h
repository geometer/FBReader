/*
 * FBReader -- electronic book reader
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

#ifndef __TXTREADER_H__
#define __TXTREADER_H__

#include <string>

#include <abstract/EncodingConverter.h>

class ZLInputStream;

class TxtReader {

public:
	void readDocument(ZLInputStream &stream) FORMATS_SECTION;

protected:
	TxtReader(const std::string &encoding) FORMATS_SECTION;
	virtual ~TxtReader() FORMATS_SECTION;

protected:
	virtual void startDocumentHandler() FORMATS_SECTION = 0;
	virtual void endDocumentHandler() FORMATS_SECTION = 0;

	virtual bool characterDataHandler(std::string &str) FORMATS_SECTION = 0;
	virtual bool newLineHandler() FORMATS_SECTION = 0;

private:
	EncodingConverter myConverter;
};

#endif /* __TXTREADER_H__ */
