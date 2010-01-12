/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __PdfBOOKREADER_H__
#define __PdfBOOKREADER_H__

#include <map>

#include "../../bookmodel/BookReader.h"

class PdfObject;
class PdfObjectReference;

class PdfBookReader {

public:
	PdfBookReader(BookModel &model);
	~PdfBookReader();
	bool readBook(shared_ptr<ZLInputStream> stream);

private:
	bool readReferenceTable(ZLInputStream &stream, int offset);
	shared_ptr<PdfObject> resolveReference(shared_ptr<PdfObject> reference, ZLInputStream &stream);
	shared_ptr<PdfObject> readObjectFromLocation(ZLInputStream &stream, const std::pair<int,int> &address);
	void processPage(shared_ptr<PdfObject> pageObject, ZLInputStream &stream);
	void processContents(shared_ptr<PdfObject> contentsObject, ZLInputStream &stream);

private:
	BookReader myModelReader;
	std::string myBuffer;
	std::map<std::pair<int,int>,int> myObjectLocationMap;
	std::map<std::pair<int,int>,shared_ptr<PdfObject> > myObjectMap;
	shared_ptr<PdfObject> myTrailer;
};

#endif /* __PdfBOOKREADER_H__ */
