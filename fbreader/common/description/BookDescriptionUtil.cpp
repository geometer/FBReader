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

#include <abstract/ZLFSManager.h>
#include <abstract/ZLOptions.h>
#include <abstract/ZLStringUtil.h>
#include <abstract/ZLDir.h>

#include "BookDescriptionUtil.h"
#include "BookDescription.h"
#include "../formats/FormatPlugin.h"

static const std::string SIZE = "Size";
static const std::string MTIME = "MTime";
static const std::string ENTRY = "Entry";
static const std::string ENTRIES_NUMBER = "EntriesNumber";

bool BookDescriptionUtil::checkInfo(const ZLFile &file) {
	return
		(ZLIntegerOption(file.path(), SIZE, -1).value() == (int)file.size()) &&
		(ZLIntegerOption(file.path(), MTIME, -1).value() == (int)file.mTime());
}

void BookDescriptionUtil::saveInfo(const ZLFile &file) {
	ZLIntegerOption(file.path(), SIZE, -1).setValue(file.size());
	ZLIntegerOption(file.path(), MTIME, -1).setValue(file.mTime());
}

void BookDescriptionUtil::listZipEntries(const ZLFile &zipFile, std::vector<std::string> &entries) {
	int entriesNumber = ZLIntegerOption(zipFile.path(), ENTRIES_NUMBER, -1).value();
	if (entriesNumber == -1) {
		resetZipInfo(zipFile.path());
		entriesNumber = ZLIntegerOption(zipFile.path(), ENTRIES_NUMBER, -1).value();
	}
	for (int i = 0; i < entriesNumber; i++) {
		std::string optionName = ENTRY;
		ZLStringUtil::appendNumber(optionName, i);
		std::string entry = ZLStringOption(zipFile.path(), optionName, "").value();
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
		for (std::vector<std::string>::iterator zit = entries.begin(); zit != entries.end(); zit++) {
			if (PluginCollection::instance().plugin(ZLFile(*zit).extension(), true) != 0) {
				std::string optionName = ENTRY;
				ZLStringUtil::appendNumber(optionName, counter);
				std::string fullName = zipPrefix + *zit;
				ZLStringOption(zipFile.path(), optionName, "").setValue(fullName);
				BookInfo(fullName).reset();
				counter++;
			}
		}
		ZLIntegerOption(zipFile.path(), ENTRIES_NUMBER, -1).setValue(counter);
	}
}
