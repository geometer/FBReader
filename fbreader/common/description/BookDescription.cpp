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

#include <sys/stat.h>

#include <abstract/ZLOptions.h>

#include "BookDescription.h"
#include "DescriptionReader.h"
#include "Author.h"

#include "../formats/FormatPlugin.h"

BookDescription *BookDescription::create(const std::string &fileName) {
	BookDescription *description = new BookDescription(fileName);
	if (!description->myIsValid) {
		delete description;
		return 0;
	}
	return description;
}

BookDescription::BookDescription(const BookDescription &description) {
	myFileName = description.myFileName;
	myAuthor = description.myAuthor->createCopy();
	myTitle = description.myTitle;
	myLanguage = description.myLanguage;
	myIsValid = description.myIsValid;
}

BookDescription::BookDescription(const std::string &fileName) {
	myFileName = fileName;
	myIsValid = false;
	myAuthor = 0;

	ZLStringOption AuthorDisplayNameOption(fileName, "AuthorDisplayName", "");
	ZLStringOption AuthorSortKeyOption(fileName, "AuthorSortKey", "");
	ZLStringOption TitleOption(fileName, "Title", "");
	ZLStringOption LanguageOption(fileName, "Language", "unknown");

	std::string realFileName = fileName.substr(0, fileName.find(':'));
	ZLIntegerOption FileSizeOption(realFileName, "Size", -1);
	ZLIntegerOption FileMTimeOption(realFileName, "MTime", -1);

	struct stat fileStat;
	stat(realFileName.c_str(), &fileStat);
	if ((fileStat.st_size == FileSizeOption.value()) && (fileStat.st_mtime == FileMTimeOption.value())) {
		const std::string &displayName = AuthorDisplayNameOption.value();
		const std::string &sortKey = AuthorSortKeyOption.value();
		const std::string &title = TitleOption.value();
		if (!displayName.empty() && !sortKey.empty() && !title.empty()) {
			myAuthor = new StoredAuthor(displayName, sortKey);
			myTitle = title;
			myLanguage = LanguageOption.value();
			myIsValid = true;
			return;
		}
	} else {
		FileSizeOption.setValue(fileStat.st_size);
		FileMTimeOption.setValue(fileStat.st_mtime);
	}

	FormatPlugin *plugin = PluginCollection::instance().plugin(myFileName, false);
	if (plugin != 0) {
		plugin->readDescription(myFileName, *this);
	}

	if (!myIsValid) {
		return;
	}
	if (myTitle.empty()) {
		int slashPos = myFileName.find_last_of('/');
		int dotPos = myFileName.find_first_of('.', slashPos + 1);
		myTitle = myFileName.substr(slashPos + 1, dotPos - slashPos - 1);
	}
	if (myAuthor == 0) {
		myAuthor = new DummyAuthor();
	}
	AuthorDisplayNameOption.setValue(myAuthor->displayName());
	AuthorSortKeyOption.setValue(myAuthor->sortKey());
	TitleOption.setValue(myTitle);
	LanguageOption.setValue(myLanguage);
}

BookDescription::~BookDescription() {
	if (myAuthor != 0) {
		delete myAuthor;
	}
}
