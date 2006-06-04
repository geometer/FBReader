/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLOptions.h>
#include <abstract/ZLFSManager.h>
#include <abstract/ZLUnicodeUtil.h>

#include "BookDescription.h"
#include "BookDescriptionUtil.h"
#include "Author.h"

#include "../formats/FormatPlugin.h"

#include "../FBOptions.h"

BookInfo::BookInfo(const std::string &fileName) : 
	AuthorDisplayNameOption(FBOptions::BOOKS_CATEGORY, fileName, "AuthorDisplayName", ""),
	AuthorSortKeyOption(FBOptions::BOOKS_CATEGORY, fileName, "AuthorSortKey", ""),
	TitleOption(FBOptions::BOOKS_CATEGORY, fileName, "Title", ""),
	LanguageOption(FBOptions::BOOKS_CATEGORY, fileName, "Language", "unknown"),
	EncodingOption(FBOptions::BOOKS_CATEGORY, fileName, "Encoding", "") {
}

void BookInfo::reset() {
	AuthorDisplayNameOption.setValue("");
	AuthorSortKeyOption.setValue("");
	TitleOption.setValue("");
	LanguageOption.setValue("unknown");
	EncodingOption.setValue("");
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
	int index = fileName.find(':');
	ZLFile file((index == -1) ? fileName : fileName.substr(0, index));
	if (!file.exists()) {
		return 0;
	}

	BookDescriptionPtr description = new BookDescription(fileName);

	if (BookDescriptionUtil::checkInfo(file)) {
		BookInfo info(fileName);
		if (info.isFull()) {
			description->myAuthor = SingleAuthor::create(info.AuthorDisplayNameOption.value(), info.AuthorSortKeyOption.value());
			description->myTitle = info.TitleOption.value();
			description->myLanguage = info.LanguageOption.value();
			description->myEncoding = info.EncodingOption.value();
			return description;
		}
	} else {
		if (index != -1) {
			BookDescriptionUtil::resetZipInfo(file);
		}
		BookDescriptionUtil::saveInfo(file);
	}

	FormatPlugin *plugin = PluginCollection::instance().plugin(ZLFile(fileName), false);
	if ((plugin == 0) || !plugin->readDescription(fileName, *description)) {
		return 0;
	}

	if (description->myTitle.empty()) {
		int slashPos = fileName.find('/');
		int dotPos = fileName.rfind('.', slashPos + 1);
		description->myTitle = fileName.substr(slashPos + 1, dotPos - slashPos - 1);
	}
	if (description->myAuthor == 0) {
		description->myAuthor = SingleAuthor::create();
	}
	if (description->myEncoding.empty()) {
		description->myEncoding = "auto";
	}
	{
		BookInfo info(fileName);
		info.AuthorDisplayNameOption.setValue(description->myAuthor->displayName());
		info.AuthorSortKeyOption.setValue(description->myAuthor->sortKey());
		info.TitleOption.setValue(description->myTitle);
		info.LanguageOption.setValue(description->myLanguage);
		info.EncodingOption.setValue(description->myEncoding);
	}
	return description;
}

BookDescription::BookDescription(const std::string &fileName) {
	myFileName = fileName;
	myAuthor = 0;
}

void WritableBookDescription::addAuthor(const std::string &name) {
  int stripIndex = name.length() - 1;
  while ((stripIndex >= 0) && (name[stripIndex] == ' ')) {
    stripIndex--;
  }
  std::string strippedName = name.substr(0, stripIndex + 1);
  int index = strippedName.rfind(' ');
  if (index == -1) {
    addAuthor(strippedName, strippedName);
  } else {
    std::string lastName = strippedName.substr(index + 1);
    while ((index >= 0) && (strippedName[index] == ' ')) {
      index--;
    }
    std::string firstName = strippedName.substr(0, index + 1);
    addAuthor(firstName + ' ' + lastName, lastName);
  }
}

void WritableBookDescription::addAuthor(const std::string &name, const std::string &sortKey) {
	AuthorPtr author = SingleAuthor::create(name, ZLUnicodeUtil::toLower(sortKey));
	if (myDescription.myAuthor.isNull()) {
		myDescription.myAuthor = author;
	} else {
		if (myDescription.myAuthor->isSingle()) {
		  myDescription.myAuthor = MultiAuthor::create(myDescription.myAuthor);
		}
		((MultiAuthor&)*myDescription.myAuthor).addAuthor(author);
	}
}
