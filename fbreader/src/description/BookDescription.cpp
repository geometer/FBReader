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

#include <algorithm>

#include <ZLOptions.h>
#include <ZLFile.h>
#include <ZLUnicodeUtil.h>
#include <ZLStringUtil.h>

#include "BookDescription.h"
#include "BookDescriptionUtil.h"
#include "Author.h"

#include "../formats/FormatPlugin.h"
#include "../options/FBOptions.h"

std::map<std::string,BookDescriptionPtr> BookDescription::ourDescriptions;

static const std::string EMPTY = "";

BookInfo::BookInfo(const std::string &fileName) : 
	AuthorDisplayNameOption(FBCategoryKey::BOOKS, fileName, "AuthorDisplayName", EMPTY),
	AuthorSortKeyOption(FBCategoryKey::BOOKS, fileName, "AuthorSortKey", EMPTY),
	TitleOption(FBCategoryKey::BOOKS, fileName, "Title", EMPTY),
	SequenceNameOption(FBCategoryKey::BOOKS, fileName, "Sequence", EMPTY),
	NumberInSequenceOption(FBCategoryKey::BOOKS, fileName, "Number in seq", 0, 100, 0),
	LanguageOption(FBCategoryKey::BOOKS, fileName, "Language", PluginCollection::instance().DefaultLanguageOption.value()),
	EncodingOption(FBCategoryKey::BOOKS, fileName, "Encoding", EMPTY),
	IsSequenceDefinedOption(FBCategoryKey::BOOKS, fileName, "SequenceDefined", ZLFile(fileName).extension() != "fb2") {
	// this is just hack for compatibility with versions < 0.8.8
	std::string language = LanguageOption.value();
	if (language == "") {
		LanguageOption.setValue(PluginCollection::instance().DefaultLanguageOption.value());
	} else if (language == "cz") {
		LanguageOption.setValue("cs");
	} else if (language == "none") {
		LanguageOption.setValue("other");
	} else if ((language == "chinese") || (language == "anycharacter")) {
		LanguageOption.setValue("zh");
	}
}

void BookInfo::reset() {
	AuthorDisplayNameOption.setValue(EMPTY);
	AuthorSortKeyOption.setValue(EMPTY);
	TitleOption.setValue(EMPTY);
	SequenceNameOption.setValue(EMPTY);
	NumberInSequenceOption.setValue(0);
	LanguageOption.setValue(PluginCollection::instance().DefaultLanguageOption.value());
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
	const std::string physicalFileName = ZLFile(fileName).physicalFilePath();
	ZLFile file(physicalFileName);
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
		description->myAuthor = SingleAuthor::create(info.AuthorDisplayNameOption.value(), info.AuthorSortKeyOption.value());
		description->myTitle = info.TitleOption.value();
		description->mySequenceName = info.SequenceNameOption.value();
		description->myNumberInSequence = info.NumberInSequenceOption.value();
		description->myLanguage = info.LanguageOption.value();
		description->myEncoding = info.EncodingOption.value();
		if (info.isFull()) {
			return description;
		}
	} else {
		if (physicalFileName != fileName) {
			BookDescriptionUtil::resetZipInfo(file);
		}
		BookDescriptionUtil::saveInfo(file);
	}

	ZLFile bookFile(fileName);
	FormatPlugin *plugin = PluginCollection::instance().plugin(bookFile, false);
	if ((plugin == 0) || !plugin->readDescription(fileName, *description)) {
		return 0;
	}

	if (description->myTitle.empty()) {
		description->myTitle = ZLFile::fileNameToUtf8(bookFile.name(true));
	}
	AuthorPtr author = description->myAuthor;
	if (author.isNull() || author->displayName().empty()) {
		description->myAuthor = SingleAuthor::create();
	}
	if (description->myEncoding.empty()) {
		description->myEncoding = "auto";
	}
	if (description->myLanguage.empty()) {
		description->myLanguage = PluginCollection::instance().DefaultLanguageOption.value();
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

void WritableBookDescription::clearAuthor() {
	myDescription.myAuthor = 0;
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

void WritableBookDescription::addTag(const std::string &tag) {
	std::vector<std::string> &tags = myDescription.myTags;
	std::vector<std::string>::const_iterator it = std::find(tags.begin(), tags.end(), tag);
	if (it == tags.end()) {
		tags.push_back(tag);
		// TODO: save changed tag set
	}
}

void WritableBookDescription::removeTag(const std::string &tag, bool includeSubTags) {
	std::vector<std::string> &tags = myDescription.myTags;
	if (includeSubTags) {
		const std::string prefix = tag + '/';
		for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end();) {
			if ((*it == tag) || ZLStringUtil::stringStartsWith(*it, prefix)) {
				it = tags.erase(it);
			} else {
				++it;
			}
		}
	} else {
		std::vector<std::string>::iterator it = std::find(tags.begin(), tags.end(), tag);
		if (it != tags.end()) {
			tags.erase(it);
			// TODO: save changed tag set
		}
	}
}

void WritableBookDescription::renameTag(const std::string &from, const std::string &to) {
	std::vector<std::string> &tags = myDescription.myTags;
	std::vector<std::string>::iterator it = std::find(tags.begin(), tags.end(), from);
	if (it != tags.end()) {
		std::vector<std::string>::const_iterator jt = std::find(tags.begin(), tags.end(), to);
		if (jt == tags.end()) {
			*it = to;
		} else {
			tags.erase(it);
		}
		// TODO: save changed tag set
	}
}

void WritableBookDescription::cloneTag(const std::string &from, const std::string &to) {
	std::vector<std::string> &tags = myDescription.myTags;
	std::vector<std::string>::const_iterator it = std::find(tags.begin(), tags.end(), from);
	if (it != tags.end()) {
		std::vector<std::string>::const_iterator jt = std::find(tags.begin(), tags.end(), to);
		if (jt == tags.end()) {
			tags.push_back(to);
			// TODO: save changed tag set
		}
	}
}
