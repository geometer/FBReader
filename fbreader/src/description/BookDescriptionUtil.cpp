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

#include <ZLFile.h>
#include <ZLOptions.h>
#include <ZLStringUtil.h>
#include <ZLDir.h>

#include "BookDescriptionUtil.h"
#include "BookDescription.h"
#include "../formats/FormatPlugin.h"
#include "../options/FBOptions.h"

static const std::string SIZE = "Size";
static const std::string ENTRY = "Entry";
static const std::string ENTRIES_NUMBER = "EntriesNumber";

bool BookDescriptionUtil::checkInfo(const ZLFile &file) {
	return
		(ZLIntegerOption(FBCategoryKey::BOOKS, file.path(), SIZE, -1).value() == (int)file.size());
}

void BookDescriptionUtil::saveInfo(const ZLFile &file) {
	ZLIntegerOption(FBCategoryKey::BOOKS, file.path(), SIZE, -1).setValue(file.size());
}

void BookDescriptionUtil::listZipEntries(const ZLFile &zipFile, std::vector<std::string> &entries) {
	int entriesNumber = ZLIntegerOption(FBCategoryKey::BOOKS, zipFile.path(), ENTRIES_NUMBER, -1).value();
	if (entriesNumber == -1) {
		resetZipInfo(zipFile.path());
		entriesNumber = ZLIntegerOption(FBCategoryKey::BOOKS, zipFile.path(), ENTRIES_NUMBER, -1).value();
	}
	for (int i = 0; i < entriesNumber; ++i) {
		std::string optionName = ENTRY;
		ZLStringUtil::appendNumber(optionName, i);
		std::string entry = ZLStringOption(FBCategoryKey::BOOKS, zipFile.path(), optionName, "").value();
		if (!entry.empty()) {
			entries.push_back(entry);
		}
	}
}

void BookDescriptionUtil::resetZipInfo(const ZLFile &zipFile) {
	ZLOption::clearGroup(zipFile.path());

	shared_ptr<ZLDir> zipDir = zipFile.directory();
	if (!zipDir.isNull()) {
		std::string zipPrefix = zipFile.path() + ':';
		std::vector<std::string> entries;
		int counter = 0;
		zipDir->collectFiles(entries, false);
		for (std::vector<std::string>::iterator zit = entries.begin(); zit != entries.end(); ++zit) {
			if (PluginCollection::instance().plugin(ZLFile(*zit), true) != 0) {
				std::string optionName = ENTRY;
				ZLStringUtil::appendNumber(optionName, counter);
				std::string fullName = zipPrefix + *zit;
				ZLStringOption(FBCategoryKey::BOOKS, zipFile.path(), optionName, "").setValue(fullName);
				BookInfo(fullName).reset();
				++counter;
			}
		}
		ZLIntegerOption(FBCategoryKey::BOOKS, zipFile.path(), ENTRIES_NUMBER, -1).setValue(counter);
	}
}

void BookDescriptionUtil::removeWhiteSpacesFromTag(std::string &tag) {
	int index = tag.find('/');
	if (index == -1) {
		ZLStringUtil::stripWhiteSpaces(tag);
	} else {
		std::string result;
		int index0 = 0;
		while (true) {
			std::string subtag = tag.substr(index0, index - index0); 
			ZLStringUtil::stripWhiteSpaces(subtag);
			if (!subtag.empty()) {
				if (!result.empty()) {
					result += '/';
				}
				result += subtag;
			}
			if (index == -1) {
				break;
			}
			index0 = index + 1;
			index = tag.find('/', index0);
		}
		tag = result;
	}
}
