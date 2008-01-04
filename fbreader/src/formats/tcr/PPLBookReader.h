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

#ifndef __PPLBOOKREADER_H__
#define __PPLBOOKREADER_H__

#include <shared_ptr.h>
#include <ZLEncodingConverter.h>
#include "../../bookmodel/BookReader.h"
#include "../EncodedTextReader.h"

class ZLInputStream;
class BookModel;

class PPLBookReader : public EncodedTextReader {

public:
	PPLBookReader(BookModel &model, const std::string &encoding);
	~PPLBookReader();

	bool readDocument(ZLInputStream &stream);

private:
	bool currentParagraphIsEmpty() const;
	void addParagraph();

private:
	BookReader myModelReader;

	char *myBuffer;
	std::string myCurrentParagraph;
	int myEmptyLineCounter;
};

#endif /* __PPLBOOKREADER_H__ */
