/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLInputStream.h>
#include <ZLApplication.h>

#include "XMLConfig.h"
#include "XMLConfigDelta.h"
#include "XMLConfigReader.h"
#include "XMLConfigWriter.h"
#include "XMLConfigDeltaWriter.h"

const std::string XMLConfig::UNKNOWN_CATEGORY = ".unknown.";

static const std::string CHANGES_FILE = "config.changes";

std::string XMLConfig::configDirName() const {
	return ZLApplication::HomeDirectory + ZLApplication::FileNameDelimiter + "." + ZLApplication::ApplicationName();
}

void XMLConfig::load() {
	XMLConfigReader(*this, "").readDocument(ZLApplication::DefaultFilesPathPrefix() + "config.xml");
	shared_ptr<ZLDir> configDir = ZLFile(configDirName()).directory(false);
	if (configDir.isNull()) {
		return;
	}
	std::vector<std::string> fileNames;
	configDir->collectFiles(fileNames, true);
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
		ZLFile configFile(configDir->itemName(*it));
		if (configFile.extension() == "xml") {
			XMLConfigReader(*this, configFile.name()).readDocument(configFile.inputStream());
		}
	}
	if (myDelta == 0) {
		myDelta = new XMLConfigDelta();
	}
	XMLConfigReader(*this, UNKNOWN_CATEGORY).readDocument(configDir->itemName(CHANGES_FILE));
}

void XMLConfig::saveAll() {
	saveDelta();

	shared_ptr<ZLDir> configDir = ZLFile(configDirName()).directory(true);

	if (myDelta != 0) {
		if (!configDir.isNull()) {
			std::set<std::string> &categories = myDelta->myCategories;
			for (std::set<std::string>::const_iterator it = categories.begin(); it != categories.end(); ++it) {
				if (!it->empty()) {
	ZLTime t;
					shared_ptr<ZLOutputStream> stream = ZLFile(configDir->itemName(*it + ".xml")).outputStream();
					if (!stream.isNull() && stream->open()) {
						XMLConfigWriter(*this, *stream, *it).write();
						stream->close();
					}
				}
			}
		}
		myDelta->clear();
	} // TODO: show error message if config was not saved
	ZLFile changesFile(configDirName() + ZLApplication::FileNameDelimiter + CHANGES_FILE);
	changesFile.remove();
}

void XMLConfig::saveDelta() {
	if ((myDelta == 0) || (myDelta->myIsUpToDate)) {
		return;
	}
	shared_ptr<ZLDir> configDir = ZLFile(configDirName()).directory(true);
	shared_ptr<ZLOutputStream> stream = ZLFile(configDir->itemName(CHANGES_FILE)).outputStream();
	if (!stream.isNull() && stream->open()) {
		XMLConfigDeltaWriter(*myDelta, *stream).write();
		stream->close();
	}
	myDelta->myIsUpToDate = true;
}
