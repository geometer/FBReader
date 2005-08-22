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

#ifndef __FB2DESCRIPTIONREADER_H__
#define __FB2DESCRIPTIONREADER_H__

#include <string>

#include "FB2Reader.h"
#include "../../description/BookDescription.h"

class FB2DescriptionReader : public FB2Reader {

public:
	FB2DescriptionReader(BookDescription &description) FORMATS_SECTION;
	~FB2DescriptionReader() FORMATS_SECTION;
	bool readDescription(shared_ptr<ZLInputStream> stream) FORMATS_SECTION;

	void startElementHandler(int tag, const char **attributes) FORMATS_SECTION;
	void endElementHandler(int tag) FORMATS_SECTION;
	void characterDataHandler(const char *text, int len) FORMATS_SECTION;

private:
	WritableBookDescription myDescription;

	bool myReturnCode;

	bool myReadSomething;
	bool myReadTitle;
	bool myReadAuthor;
	bool myReadAuthorName[3];
	bool myReadLanguage;

	std::string myAuthorNames[3];
};

inline FB2DescriptionReader::~FB2DescriptionReader() {}

#endif /* __FB2DESCRIPTIONREADER_H__ */
