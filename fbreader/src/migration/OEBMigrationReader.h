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

#ifndef __OEBMIGRATIONREADER_H__
#define __OEBMIGRATIONREADER_H__

#include <vector>

#include <ZLXMLReader.h>

#include "../description/BookDescription.h"

class OEBMigrationReader : public ZLXMLReader {

public:
	OEBMigrationReader(BookInfo &info);
	void doRead(const std::string &fileName);

	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, int len);
	bool processNamespaces() const;

private:
	bool isDublinCoreNamespace(const std::string &nsId) const;

private:
	BookInfo &myInfo;

	bool myReadMetaData;
	bool myReadSubject;

	std::string myDCMetadataTag;
	std::string myBuffer;
	std::string myTagList;
};

#endif /* __OEBMIGRATIONREADER_H__ */
