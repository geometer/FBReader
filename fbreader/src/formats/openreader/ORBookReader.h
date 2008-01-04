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

#ifndef __ORBOOKREADER_H__
#define __ORBOOKREADER_H__

#include <map>
#include <set>
#include <vector>
#include <string>

#include <ZLXMLReader.h>

#include "../../bookmodel/BookReader.h"

class ORBookReader : public ZLXMLReader {

public:
	ORBookReader(BookModel &model);
	bool readBook(const std::string &fileName);

	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, int len);

private:
	enum ReaderState {
		READ_NONE,
		READ_RESOURCES,
		READ_USERSET,
		READ_NAVIGATION,
		READ_POINTER,
		READ_TOCTITLE
	};

	BookReader myModelReader;
	ReaderState myState;

	std::string myFilePrefix;
	std::map<std::string,std::string> myResources;
	std::string myCoverReference;
	std::set<std::string> myHtmlFileIDs;
	std::map<std::string,std::string> myImageIDs;
	std::vector<std::string> myHtmlFilesOrder;

	struct TOCItem {
		TOCItem(const std::string &reference, const std::string &text, int level) : Reference(reference), Text(text), Level(level) {
		}

		std::string Reference;
		std::string Text;
		int Level;
	};
	std::vector<TOCItem> myTOC;

	std::string myTOCReference;
	int myTOCLevel;
	std::string myTOCTitle;
};

#endif /* __ORBOOKREADER_H__ */
