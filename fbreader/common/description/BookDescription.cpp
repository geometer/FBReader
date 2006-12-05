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

#include <ZLOptions.h>
#include <ZLFile.h>
#include <ZLUnicodeUtil.h>
#include <ZLStringUtil.h>

#include "BookDescription.h"
#include "BookDescriptionUtil.h"
#include "Author.h"

#include "../formats/FormatPlugin.h"

#include "../FBOptions.h"

std::map<std::string,BookDescriptionPtr> BookDescription::ourDescriptions;

static const std::string EMPTY = "";
static const std::string UNKNOWN = "unknown";

BookInfo::BookInfo(const std::string &fileName) : 
	AuthorDisplayNameOption(FBOptions::BOOKS_CATEGORY, fileName, "AuthorDisplayName", EMPTY),
	AuthorSortKeyOption(FBOptions::BOOKS_CATEGORY, fileName, "AuthorSortKey", EMPTY),
	TitleOption(FBOptions::BOOKS_CATEGORY, fileName, "Title", EMPTY),
	SequenceNameOption(FBOptions::BOOKS_CATEGORY, fileName, "Sequence", EMPTY),
	NumberInSequenceOption(FBOptions::BOOKS_CATEGORY, fileName, "Number in seq", 0, 100, 0),
	LanguageOption(FBOptions::BOOKS_CATEGORY, fileName, "Language", UNKNOWN),
	EncodingOption(FBOptions::BOOKS_CATEGORY, fileName, "Encoding", EMPTY),
	IsSequenceDefinedOption(FBOptions::BOOKS_CATEGORY, fileName, "SequenceDefined", ZLFile(fileName).extension() != "fb2") {
}

void BookInfo::reset() {
	AuthorDisplayNameOption.setValue(EMPTY);
	AuthorSortKeyOption.setValue(EMPTY);
	TitleOption.setValue(EMPTY);
	SequenceNameOption.setValue(EMPTY);
	NumberInSequenceOption.setValue(0);
	LanguageOption.setValue(UNKNOWN);
	EncodingOption.setValue(EMPTY);
}

bool BookInfo::isFull() const {
	return
		!AuthorDisplayNameOption.value().empty() &&
		!AuthorSortKeyOption.value().empty() &&
		!TitleOption.value().empty() &&
		!EncodingOption.value().empty() &&
		IsSequenceDefinedOption.value();
}

BookDescriptionPtr BookDescription::getDescription(const std::string &fileName, bool checkFile) {
	int index = fileName.find(':');
	ZLFile file((index == -1) ? fileName : fileName.substr(0, index));
	if (checkFile && !file.exists()) {
		return 0;
	}

	BookDescriptionPtr description = ourDescriptions[fileName];
	if (description.isNull()) {
		description = new BookDescription(fileName);
		ourDescriptions[fileName] = description;
	}

	if (!checkFile || BookDescriptionUtil::checkInfo(file)) {
		BookInfo info(fileName);
		if (info.isFull()) {
			description->myAuthor = SingleAuthor::create(info.AuthorDisplayNameOption.value(), info.AuthorSortKeyOption.value());
			description->myTitle = info.TitleOption.value();
			description->mySequenceName = info.SequenceNameOption.value();
			description->myNumberInSequence = info.NumberInSequenceOption.value();
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
		info.SequenceNameOption.setValue(description->mySequenceName);
		info.NumberInSequenceOption.setValue(description->myNumberInSequence);
		info.LanguageOption.setValue(description->myLanguage);
		info.EncodingOption.setValue(description->myEncoding);
		info.IsSequenceDefinedOption.setValue(true);
	}
	return description;
}

BookDescription::BookDescription(const std::string &fileName) {
	myFileName = fileName;
	myAuthor = 0;
	myNumberInSequence = 0;
}

void WritableBookDescription::addAuthor(const std::string &name, const std::string &sortKey) {
	std::string strippedName = name;
	ZLStringUtil::stripWhiteSpaces(strippedName);
	if (strippedName.empty()) {
		return;
	}

	std::string strippedKey = sortKey;
	ZLStringUtil::stripWhiteSpaces(strippedKey);
	if (strippedKey.empty()) {
		int index = strippedName.rfind(' ');
		if (index == -1) {
			strippedKey = strippedName;
		} else {
			strippedKey = strippedName.substr(index + 1);
			while ((index >= 0) && (strippedName[index] == ' ')) {
				--index;
			}
			strippedName = strippedName.substr(0, index + 1) + ' ' + strippedKey;
		}
	}
	AuthorPtr author = SingleAuthor::create(strippedName, ZLUnicodeUtil::toLower(strippedKey));
	if (myDescription.myAuthor.isNull()) {
		myDescription.myAuthor = author;
	} else {
		if (myDescription.myAuthor->isSingle()) {
			myDescription.myAuthor = MultiAuthor::create(myDescription.myAuthor);
		}
		((MultiAuthor&)*myDescription.myAuthor).addAuthor(author);
	}
}
