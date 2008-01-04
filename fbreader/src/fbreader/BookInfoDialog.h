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

#ifndef __BOOKINFODIALOG_H__
#define __BOOKINFODIALOG_H__

#include <string>

#include <ZLOptionEntry.h>

#include "../collection/BookCollection.h"
#include "../description/BookDescription.h"
#include "../formats/FormatPlugin.h"

class ZLOptionsDialog;
class AuthorDisplayNameEntry;
class AuthorSortKeyEntry;
class SeriesTitleEntry;

class BookInfoDialog {

public:
	BookInfoDialog(const BookCollection &collection, const std::string &fileName);

	ZLOptionsDialog &dialog();

private:
	shared_ptr<ZLOptionsDialog> myDialog;
	const BookCollection &myCollection;
	BookInfo myBookInfo;
	shared_ptr<FormatInfoPage> myFormatInfoPage;

	AuthorDisplayNameEntry *myAuthorDisplayNameEntry;
	AuthorSortKeyEntry *myAuthorSortKeyEntry;
	ZLComboOptionEntry *myEncodingSetEntry;
	ZLComboOptionEntry *myEncodingEntry;
	ZLComboOptionEntry *myLanguageEntry;
	SeriesTitleEntry *mySeriesTitleEntry;
	ZLSpinOptionEntry *myBookNumberEntry;

friend class AuthorDisplayNameEntry;
friend class AuthorSortKeyEntry;
friend class SeriesTitleEntry;
};

inline ZLOptionsDialog &BookInfoDialog::dialog() { return *myDialog; }

#endif /* __BOOKINFODIALOG_H__ */
