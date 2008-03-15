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
#include <set>

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
	SeriesNameOption(FBCategoryKey::BOOKS, fileName, "Sequence", EMPTY),
	NumberInSeriesOption(FBCategoryKey::BOOKS, fileName, "Number in seq", 0, 100, 0),
	LanguageOption(FBCategoryKey::BOOKS, fileName, "Language", PluginCollection::instance().DefaultLanguageOption.value()),
	EncodingOption(FBCategoryKey::BOOKS, fileName, "Encoding", EMPTY),
	TagsOption(FBCategoryKey::BOOKS, fileName, "TagList", EMPTY) {
}

void BookInfo::reset() {
	AuthorDisplayNameOption.setValue(EMPTY);
	AuthorSortKeyOption.setValue(EMPTY);
	TitleOption.setValue(EMPTY);
	SeriesNameOption.setValue(EMPTY);
	NumberInSeriesOption.setValue(0);
	LanguageOption.setValue(PluginCollection::instance().DefaultLanguageOption.value());
	EncodingOption.setValue(EMPTY);
	TagsOption.setValue(EMPTY);
}

bool BookInfo::isFull() const {
	return
		!AuthorDisplayNameOption.value().empty() &&
		!AuthorSortKeyOption.value().empty() &&
		!TitleOption.value().empty() &&
		!EncodingOption.value().empty();
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
		description->mySeriesName = info.SeriesNameOption.value();
		description->myNumberInSeries = info.NumberInSeriesOption.value();
		description->myLanguage = info.LanguageOption.value();
		description->myEncoding = info.EncodingOption.value();
		description->myTags.clear();
		const std::string &tagList = info.TagsOption.value();
		if (!tagList.empty()) {
			int index = 0;
			do {
				int newIndex = tagList.find(',', index);
				description->addTag(tagList.substr(index, newIndex - index));
				index = newIndex + 1;
			} while (index != 0);
		}
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
		info.SeriesNameOption.setValue(description->mySeriesName);
		info.NumberInSeriesOption.setValue(description->myNumberInSeries);
		info.LanguageOption.setValue(description->myLanguage);
		info.EncodingOption.setValue(description->myEncoding);
		description->saveTags(info.TagsOption);
	}
	return description;
}

BookDescription::BookDescription(const std::string &fileName) {
	myFileName = fileName;
	myAuthor = 0;
	myNumberInSeries = 0;
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

void WritableBookDescription::addTag(const std::string &tag, bool check) {
	if (myDescription.addTag(tag, check)) {
		myDescription.saveTags();
	}
}

void BookDescription::saveTags() const {
	ZLStringOption tagsOption(FBCategoryKey::BOOKS, fileName(), "TagList", "");
	saveTags(tagsOption);
}

void BookDescription::saveTags(ZLStringOption &tagsOption) const {
	std::string tagList;
	if (!myTags.empty()) {
		for (std::vector<std::string>::const_iterator it = myTags.begin(); it != myTags.end(); ++it) {
			if (it != myTags.begin()) {
				tagList += ",";
			}
			tagList += *it;
		}
	}
	tagsOption.setValue(tagList);
}

bool BookDescription::addTag(const std::string &tag, bool check) {
	std::string checkedTag = tag;
	if (check) {
		BookDescriptionUtil::removeWhiteSpacesFromTag(checkedTag);
	}

	if (!checkedTag.empty()) {
		std::vector<std::string>::const_iterator it = std::find(myTags.begin(), myTags.end(), checkedTag);
		if (it == myTags.end()) {
			myTags.push_back(checkedTag);
			return true;
		}
	}
	return false;
}

void WritableBookDescription::removeTag(const std::string &tag, bool includeSubTags) {
	std::vector<std::string> &tags = myDescription.myTags;
	if (includeSubTags) {
		const std::string prefix = tag + '/';
		bool changed = false;
		for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end();) {
			if ((*it == tag) || ZLStringUtil::stringStartsWith(*it, prefix)) {
				it = tags.erase(it);
				changed = true;
			} else {
				++it;
			}
		}
		if (changed) {
			myDescription.saveTags();
		}
	} else {
		std::vector<std::string>::iterator it = std::find(tags.begin(), tags.end(), tag);
		if (it != tags.end()) {
			tags.erase(it);
			myDescription.saveTags();
		}
	}
}

void WritableBookDescription::renameTag(const std::string &from, const std::string &to, bool includeSubTags) {
	std::vector<std::string> &tags = myDescription.myTags;
	if (includeSubTags) {
		const std::string prefix = from + "/";
		std::set<std::string> tagSet;
		bool changed = false;
		for (std::vector<std::string>::const_iterator it = tags.begin(); it != tags.end(); ++it) {
			if (*it == from) {
				tagSet.insert(to);
				changed = true;
			} else if (ZLStringUtil::stringStartsWith(*it, prefix)) {
				tagSet.insert(to + "/" + it->substr(prefix.length()));
				changed = true;
			} else {
				tagSet.insert(*it);
			}
		}
		if (changed) {
			tags.clear();
			tags.insert(tags.end(), tagSet.begin(), tagSet.end());
			myDescription.saveTags();
		}
	} else {
		std::vector<std::string>::iterator it = std::find(tags.begin(), tags.end(), from);
		if (it != tags.end()) {
			std::vector<std::string>::const_iterator jt = std::find(tags.begin(), tags.end(), to);
			if (jt == tags.end()) {
				*it = to;
			} else {
				tags.erase(it);
			}
			myDescription.saveTags();
		}
	}
}

void WritableBookDescription::cloneTag(const std::string &from, const std::string &to, bool includeSubTags) {
	std::vector<std::string> &tags = myDescription.myTags;
	if (includeSubTags) {
		const std::string prefix = from + "/";
		std::set<std::string> tagSet;
		for (std::vector<std::string>::const_iterator it = tags.begin(); it != tags.end(); ++it) {
			if (*it == from) {
				tagSet.insert(to);
			} else if (ZLStringUtil::stringStartsWith(*it, prefix)) {
				tagSet.insert(to + "/" + it->substr(prefix.length()));
			}
		}
		if (!tagSet.empty()) {
			tagSet.insert(tags.begin(), tags.end());
			tags.clear();
			tags.insert(tags.end(), tagSet.begin(), tagSet.end());
			myDescription.saveTags();
		}
	} else {
		std::vector<std::string>::const_iterator it = std::find(tags.begin(), tags.end(), from);
		if (it != tags.end()) {
			std::vector<std::string>::const_iterator jt = std::find(tags.begin(), tags.end(), to);
			if (jt == tags.end()) {
				tags.push_back(to);
				myDescription.saveTags();
			}
		}
	}
}

void WritableBookDescription::removeAllTags() {
	myDescription.myTags.clear();
}
