/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __OPTIONSDIALOG_H__
#define __OPTIONSDIALOG_H__

#include <string>

#include <ZLOptionEntry.h>
#include <ZLRunnable.h>

#include "../formats/FormatPlugin.h"

class ZLOptionsDialog;
class ZLDialogContent;

class AuthorDisplayNameEntry;
class BookTagEntry;
class OptionsPage;
class SeriesTitleEntry;
class SeriesIndexEntry;

class OptionsDialog {

public:
	class ApplyAction : public ZLRunnable {

	public:
		ApplyAction(OptionsDialog &dialog);
		void run();

	private:
		OptionsDialog &myInfoDialog;
	};

public:
	OptionsDialog(shared_ptr<Book> book);
	~OptionsDialog();

	ZLOptionsDialog &dialog();

private:
	void initAuthorEntries();
	void initTagEntries();

private:
	void createIndicatorTab();
	void createKeyBindingsTab();

private:
	shared_ptr<ZLOptionsDialog> myDialog;
	shared_ptr<Book> myBook;
	shared_ptr<FormatInfoPage> myFormatInfoPage;

	ZLComboOptionEntry *myEncodingSetEntry;
	ZLComboOptionEntry *myEncodingEntry;
	ZLComboOptionEntry *myLanguageEntry;
	SeriesTitleEntry *mySeriesTitleEntry;
	SeriesIndexEntry *mySeriesIndexEntry;

	ZLDialogContent *myTagsTab;
	std::vector<BookTagEntry *> myTagEntries;
	bool myTagsDone;

	std::vector<std::string> myNewTags;

	ZLDialogContent *myAuthorsTab;
	std::vector<AuthorDisplayNameEntry *> myAuthorEntries;
	bool myAuthorsDone;

	shared_ptr<OptionsPage> myFormatPage;
	shared_ptr<OptionsPage> myStylePage;

friend class AuthorDisplayNameEntry;
friend class SeriesTitleEntry;
friend class SeriesIndexEntry;
friend class BookTitleEntry;
friend class BookEncodingEntry;
friend class BookLanguageEntry;
friend class BookTagEntry;
friend class BookInfoApplyAction;
};

inline ZLOptionsDialog &OptionsDialog::dialog() { return *myDialog; }

#endif /* __OPTIONSDIALOG_H__ */
