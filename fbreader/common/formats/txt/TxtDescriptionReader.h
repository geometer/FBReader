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

#ifndef __TXTDESCRIPTIONREADER_H__
#define __TXTDESCRIPTIONREADER_H__

#include "TxtReader.h"
#include "../../description/BookDescription.h"

class TxtDescriptionReader : public TxtReader {

public:
	TxtDescriptionReader(BookDescription &description) FORMATS_SECTION;
	~TxtDescriptionReader() FORMATS_SECTION;

protected:
	void startDocumentHandler() FORMATS_SECTION;
	void endDocumentHandler() FORMATS_SECTION;

	bool characterDataHandler(const char *text, int len) FORMATS_SECTION;
	bool newLineHandler() FORMATS_SECTION;

private:
	bool myReadTitle;
	WritableBookDescription myDescription;
};

inline TxtDescriptionReader::~TxtDescriptionReader() {}
inline bool TxtDescriptionReader::newLineHandler() { return true; }

#endif /* __TXTDESCRIPTIONREADER_H__ */
