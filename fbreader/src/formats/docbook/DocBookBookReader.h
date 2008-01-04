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

#ifndef __DOCBOOKBOOKREADER_H__
#define __DOCBOOKBOOKREADER_H__

#include "DocBookReader.h"
#include "../../bookmodel/BookReader.h"

class BookModel;

class DocBookBookReader : public BookReader, public DocBookReader {

public:
	DocBookBookReader(BookModel &model);
	~DocBookBookReader();
	void readBook(shared_ptr<ZLInputStream> stream);

	void startElementHandler(int tag, const char **attributes);
	void endElementHandler(int tag);
	void characterDataHandler(const char *text, int len);

private:
	bool myReadText;
};

inline DocBookBookReader::~DocBookBookReader() {}

#endif /* __DOCBOOKBOOKREADER_H__ */
