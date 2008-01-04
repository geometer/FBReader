/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __DOCBOOKDESCRIPTIONREADER_H__
#define __DOCBOOKDESCRIPTIONREADER_H__

#include <string>

#include "DocBookReader.h"
#include "../../description/BookDescription.h"

class DocBookDescriptionReader : public DocBookReader {

public:
	DocBookDescriptionReader(BookDescription &description);
	~DocBookDescriptionReader();
	bool readDescription(shared_ptr<ZLInputStream> stream);

	void startElementHandler(int tag, const char **attributes);
	void endElementHandler(int tag);
	void characterDataHandler(const char *text, int len);

private:
	WritableBookDescription myDescription;

	bool myReturnCode;

	bool myReadTitle;
	bool myReadAuthor;
	bool myReadAuthorName[3];

	std::string myAuthorNames[3];

	int myDepth;
};

inline DocBookDescriptionReader::~DocBookDescriptionReader() {}

#endif /* __DOCBOOKDESCRIPTIONREADER_H__ */
