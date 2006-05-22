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

#ifndef __OEBBOOKREADER_H__
#define __OEBBOOKREADER_H__

#include <map>
#include <vector>
#include <string>

#include <abstract/ZLXMLReader.h>

#include "../../bookmodel/BookReader.h"

class OEBBookReader : public ZLXMLReader {

public:
	OEBBookReader(BookModel &model) FORMATS_SECTION;
	~OEBBookReader() FORMATS_SECTION;
	bool readBook(const std::string &fileName) FORMATS_SECTION;

	void startElementHandler(const char *tag, const char **attributes) FORMATS_SECTION;
	void endElementHandler(const char *tag) FORMATS_SECTION;
	void characterDataHandler(const char *text, int len) FORMATS_SECTION;

private:
	enum ReaderState {
		READ_NONE,
		READ_MANIFEST,
		READ_SPINE,
		READ_GUIDE
	};

	BookReader myModelReader;
	ReaderState myState;

	std::string myFilePrefix;
	std::map<std::string,std::string> myIdToHref;
	std::vector<std::string> myHtmlFileNames;
	std::vector<std::pair<std::string,std::string> > myTOC;
};

inline OEBBookReader::~OEBBookReader() {}

#endif /* __OEBBOOKREADER_H__ */
