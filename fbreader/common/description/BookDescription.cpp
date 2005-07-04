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

#include <abstract/ZLOptions.h>
#include <abstract/ZLFSManager.h>

#include "BookDescription.h"
#include "Author.h"

#include "../formats/FormatPlugin.h"

BookInfo::BookInfo(const std::string &fileName) : 
	AuthorDisplayNameOption(fileName, "AuthorDisplayName", ""),
	AuthorSortKeyOption(fileName, "AuthorSortKey", ""),
	TitleOption(fileName, "Title", ""),
	LanguageOption(fileName, "Language", "unknown"),
	EncodingOption(fileName, "Encoding", "") {
}

bool BookInfo::isFull() const {
	return
		!AuthorDisplayNameOption.value().empty() &&
		!AuthorSortKeyOption.value().empty() &&
		!TitleOption.value().empty() &&
		!EncodingOption.value().empty();
}

BookDescriptionPtr BookDescription::create(const std::string &fileName) {
#ifdef PALM_TEMPORARY
	if (fileName == "$$TEST") {
		BookDescription *description = new BookDescription(fileName);
		description->myAuthor = new DummyAuthor();
		description->myLanguage = "en";
		description->myTitle = "Test text";
		description->myEncoding = "windows-1252";
		return description;
	}
#endif // PALM_TEMPORARY
	std::string realFileName = fileName.substr(0, fileName.find(':'));
	ZLFile file(realFileName);
	if (!file.exists()) {
		return 0;
	}

	BookDescriptionPtr description = new BookDescription(fileName);
	BookInfo info(fileName);
	ZLIntegerOption FileSizeOption(realFileName, "Size", -1);
	ZLIntegerOption FileMTimeOption(realFileName, "MTime", -1);

	if (((int)file.size() == FileSizeOption.value()) && ((int)file.mTime() == FileMTimeOption.value())) {
		if (info.isFull()) {
			description->myAuthor = new StoredAuthor(info.AuthorDisplayNameOption.value(), info.AuthorSortKeyOption.value());
			description->myTitle = info.TitleOption.value();
			description->myLanguage = info.LanguageOption.value();
			description->myEncoding = info.EncodingOption.value();
			return description;
		}
	} else {
		FileSizeOption.setValue(file.size());
		FileMTimeOption.setValue(file.mTime());
	}

	FormatPlugin *plugin = PluginCollection::instance().plugin(ZLFile(fileName).extension(), false);
	if ((plugin == 0) || !plugin->readDescription(fileName, *description)) {
		return 0;
	}

	if (description->myTitle.empty()) {
		int slashPos = fileName.find('/');
		int dotPos = fileName.rfind('.', slashPos + 1);
		description->myTitle = fileName.substr(slashPos + 1, dotPos - slashPos - 1);
	}
	if (description->myAuthor == 0) {
		description->myAuthor = new DummyAuthor();
	}
	if (description->myEncoding.empty()) {
		description->myEncoding = "auto";
	}
	info.AuthorDisplayNameOption.setValue(description->myAuthor->displayName());
	info.AuthorSortKeyOption.setValue(description->myAuthor->sortKey());
	info.TitleOption.setValue(description->myTitle);
	info.LanguageOption.setValue(description->myLanguage);
	info.EncodingOption.setValue(description->myEncoding);
	return description;
}

BookDescription::BookDescription(const std::string &fileName) {
	myFileName = fileName;
	myAuthor = 0;
}

BookDescription::~BookDescription() {
	if (myAuthor != 0) {
		delete myAuthor;
	}
}

void WritableBookDescription::addAuthor(const std::string &firstName, const std::string &middleName, const std::string &lastName) {
	Author *author = new SingleAuthorWith3Names(firstName, middleName, lastName);
	if (myDescription.myAuthor == 0) {
		myDescription.myAuthor = author;
	} else if (myDescription.myAuthor->isSingle()) {
		MultiAuthor *multiAuthor = new MultiAuthor();
		multiAuthor->addAuthor(myDescription.myAuthor);
		multiAuthor->addAuthor(author);
		myDescription.myAuthor = multiAuthor;
	} else {
		((MultiAuthor*)myDescription.myAuthor)->addAuthor(author);
	}
}
