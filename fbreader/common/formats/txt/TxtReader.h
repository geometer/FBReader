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
	TxtTextConverter(const char *encoding) : ZLXMLReader(encoding) {}
	void convertBuffer(std::vector<std::string> &buffer);
	void convertString(std::string &string);
	
public:
	void startElementHandler(int tag, const char **attributes);
	void endElementHandler(int tag);
	void characterDataHandler(const char *text, int len);

private:
	std::vector<std::string> *myBuffer;
};

class TxtReader {

public:
	void readDocument(ZLInputStream &stream, const std::string &encoding);

protected:
	TxtReader();
	virtual ~TxtReader();

protected:
	virtual void startDocumentHandler() = 0;
	virtual void endDocumentHandler() = 0;

	virtual bool characterDataHandler(const char *text, int len) = 0;

protected:
	TxtTextConverter *myConverter;
};

#endif /* __TXTREADER_H__ */
