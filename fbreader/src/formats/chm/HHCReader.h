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

#ifndef __HHCREADER_H__
#define __HHCREADER_H__

#include <vector>

#include "../html/HtmlReader.h"
#include "../../bookmodel/BookModel.h"
#include "../../bookmodel/BookReader.h"

class CHMReferenceCollection;

class HHCReader : public HtmlReader {

public:
	HHCReader(CHMReferenceCollection &collection, BookModel &model, const std::string &encoding);
	~HHCReader();

	void setReferences();

private:
	void startDocumentHandler();
	void endDocumentHandler();

	bool tagHandler(const HtmlTag &tag);
	bool characterDataHandler(const char*, int, bool);

private:
	CHMReferenceCollection &myReferenceCollection;

	std::string myText;
	std::string myReference;

	BookReader myBookReader;

	std::vector<std::string> myReferenceVector;
};

#endif /* __HHCREADER_H__ */
