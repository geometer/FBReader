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

#include <stdio.h>

#include <abstract/ZLOptions.h>

#include "BookList.h"

static std::string GROUP = "BookList";
static std::string SIZE = "Size";

BookList::BookList() {
	int size = ZLIntegerOption(GROUP, SIZE, 0).value();
	char optionName[12];
	for (int i = 0; i < size; i++) {
		sprintf(optionName, "Book%d", i);
		ZLStringOption bookOption(GROUP, optionName, "");
		const std::string &fileName = bookOption.value();
		if (!fileName.empty()) {
			addFileName(fileName);
		}
	}
}

BookList::~BookList() {
	ZLIntegerOption(GROUP, SIZE, 0).setValue(myFileNames.size());
	int i = 0;
	char optionName[12];
	for (std::set<std::string>::const_iterator it = myFileNames.begin(); it != myFileNames.end(); i++, it++) {
		sprintf(optionName, "Book%d", i);
		ZLStringOption(GROUP, optionName, "").setValue(*it);
	}
}

void BookList::addFileName(const std::string &fileName) {
	myFileNames.insert(fileName);
}

void BookList::removeFileName(const std::string &fileName) {
	myFileNames.erase(fileName);
}
