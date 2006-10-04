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

#ifndef __FB2BOOKREADER_H__
#define __FB2BOOKREADER_H__

#include "FB2Reader.h"
#include "../../bookmodel/BookReader.h"

class BookModel;
class Base64EncodedImage;

class FB2BookReader : public FB2Reader {

public:
	FB2BookReader(BookModel &model);
	~FB2BookReader();
	bool readBook(const std::string &fileName);

	void startElementHandler(int tag, const char **attributes);
	void endElementHandler(int tag);
	void characterDataHandler(const char *text, int len);

private:
	int mySectionDepth;
	int myBodyCounter;
	bool myInsideCoverpage;
	size_t myParagraphsBeforeBodyNumber;
	std::string myCoverImageReference;
	bool myInsidePoem;
	BookReader myModelReader;

	Base64EncodedImage *myCurrentImage;
	bool myProcessingImage;
	std::vector<std::string> myImageBuffer;

	bool mySectionStarted;
	bool myInsideTitle1;
};

inline FB2BookReader::~FB2BookReader() {}

#endif /* __FB2BOOKREADER_H__ */
