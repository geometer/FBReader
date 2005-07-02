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

#ifndef __HTMLBOOKREADER_H__
#define __HTMLBOOKREADER_H__

#include <stack>

#include "HtmlReader.h"
#include "../../bookmodel/BookReader.h"

class BookModel;
class PlainTextFormat;

class HtmlBookReader : public BookReader, public HtmlReader {

public:
	HtmlBookReader(BookModel &model, const PlainTextFormat &format) FORMATS_SECTION;
	~HtmlBookReader() FORMATS_SECTION;

protected:
	void startDocumentHandler() FORMATS_SECTION;
	void endDocumentHandler() FORMATS_SECTION;

	bool tagHandler(HtmlTag tag) FORMATS_SECTION;
	bool characterDataHandler(const char *text, int len) FORMATS_SECTION;

private:
	void flushTextBufferToParagraph() FORMATS_SECTION;

private:
	const PlainTextFormat &myFormat;
	int myIgnoreDataCounter;
	bool myIsPreformatted;
	bool myIsHyperlink;
	bool myIsStarted;
	std::stack<int> myListNumStack;
};

inline HtmlBookReader::~HtmlBookReader() {}

#endif /* __HTMLBOOKREADER_H__ */
