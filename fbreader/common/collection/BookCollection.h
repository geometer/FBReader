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

#ifndef __BOOKCOLLECTION_H__
#define __BOOKCOLLECTION_H__

#include <string>
#include <vector>
#include <set>
#include <map>

#include <abstract/ZLOptions.h>

class Author;
class BookDescription;

class DescriptionComparator {

public:
	bool operator() (const BookDescription *d1, const BookDescription *d2);
};

class BookCollection {

public:
	static ZLStringOption PathOption;
	static ZLBooleanOption ScanSubdirsOption;

public:
	BookCollection();
	~BookCollection();

	const std::vector<const Author*> authors() const { return myAuthors; }
	const std::vector<BookDescription*> books(const Author *author) { return myCollection[author]; }

	bool isActual() const;
	
private:
	void collectDirNames(std::set<std::string> &names);
	void addDescription(BookDescription *description);

private:
	typedef std::vector<BookDescription*> Books;

	std::vector<const Author*> myAuthors;
	std::map<const Author*,Books> myCollection;

	std::string myPath;
	bool myScanSubdirs;
};

#endif /* __BOOKCOLLECTION_H__ */
