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

#include <ZLOptions.h>
#include <ZLStringUtil.h>

#include "BookList.h"

static std::string GROUP = "BookList";
static std::string BOOK = "Book";
static std::string SIZE = "Size";

BookList::BookList() {
	int size = ZLIntegerOption(ZLCategoryKey::STATE, GROUP, SIZE, 0).value();
	for (int i = 0; i < size; ++i) {
		std::string optionName = BOOK;
		ZLStringUtil::appendNumber(optionName, i);
		ZLStringOption bookOption(ZLCategoryKey::STATE, GROUP, optionName, "");
		const std::string &fileName = bookOption.value();
		if (!fileName.empty()) {
			addFileName(fileName);
		}
	}
}

BookList::~BookList() {
	ZLIntegerOption(ZLCategoryKey::STATE, GROUP, SIZE, 0).setValue(myFileNames.size());
	int i = 0;
	for (std::set<std::string>::const_iterator it = myFileNames.begin(); it != myFileNames.end(); ++i, ++it) {
		std::string optionName = BOOK;
		ZLStringUtil::appendNumber(optionName, i);
		ZLStringOption(ZLCategoryKey::STATE, GROUP, optionName, "").setValue(*it);
	}
}

void BookList::addFileName(const std::string &fileName) {
	myFileNames.insert(fileName);
}

void BookList::removeFileName(const std::string &fileName) {
	myFileNames.erase(fileName);
}
