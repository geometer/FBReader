/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __BOOKLIST_H__
#define __BOOKLIST_H__

#include <set>
#include <string>

class BookList {

public:
	BookList() MODEL_SECTION;
	~BookList() MODEL_SECTION;

	const std::set<std::string> &fileNames() const MODEL_SECTION;
	void addFileName(const std::string &fileName) MODEL_SECTION;
	void removeFileName(const std::string &fileName) MODEL_SECTION;

private:
	std::set<std::string> myFileNames;
};

inline const std::set<std::string> &BookList::fileNames() const { return myFileNames; }

#endif /* __BOOKLIST_H__ */
