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

#ifndef __FB2MIGRATIONREADER_H__
#define __FB2MIGRATIONREADER_H__

#include <set>
#include <string>

#include "../formats/fb2/FB2Reader.h" 
#include "../description/BookDescription.h"

class FB2MigrationReader : public FB2Reader {

public:
	FB2MigrationReader(BookInfo &info, bool updateSeries);

	void doRead(const std::string &fileName);

	void startElementHandler(int tag, const char **attributes);
	void endElementHandler(int tag);
	void characterDataHandler(const char *text, int len);

private:
	BookInfo &myInfo;

	enum {
		READ_NOTHING,
		READ_SOMETHING,
		READ_GENRE
	} myReadState;

	bool myUpdateSeries;
	bool myUpdateTags;

	std::string myGenreBuffer;
	std::set<std::string> myTags;
};

#endif /* __FB2MIGRATIONREADER_H__ */
