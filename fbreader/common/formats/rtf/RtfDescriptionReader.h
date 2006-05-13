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

#ifndef __RTFDESCRIPTIONREADER_H__
#define __RTFDESCRIPTIONREADER_H__

#include <string>

#include "RtfReader.h"
#include "../../description/BookDescription.h"

class RtfDescriptionReader : public RtfReader {

public:
	RtfDescriptionReader(BookDescription &description);
	~RtfDescriptionReader();
	void startDocumentHandler();
	void endDocumentHandler();

	void startElementHandler(int tag);
	void endElementHandler(int tag);
	
	bool characterDataHandler(std::string &str);
	void flushBuffer();

	void addCharData(const char *data, size_t len, bool convert);
	void insertImage(const std::string &mimeType, const std::string &fileName, size_t startOffset, size_t size);

	void setFontProperty(FontProperty property, bool start);

private:
	WritableBookDescription myDescription;
	std::string author;
	std::string title;
	
	enum STATE
	{
	    READ_NONE,
	    READ_TITLE,
	    READ_AUTHOR,
	};
	
	int state;

	bool hasTitle;
	bool hasAuthor;
	bool hasEncoding;

	std::string outputBuffer;
};

inline RtfDescriptionReader::~RtfDescriptionReader() {}

#endif /* __RTFDESCRIPTIONREADER_H__ */
