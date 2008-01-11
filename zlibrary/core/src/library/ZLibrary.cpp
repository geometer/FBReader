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

#include <ZLTime.h>
#include <ZLDialogManager.h>
#include <ZLImageManager.h>
#include <ZLMessage.h>
#include <ZLUnicodeUtil.h>

#include "ZLibrary.h"
#include "../filesystem/ZLFSManager.h"
#include "../options/ZLConfig.h"

std::string ZLibrary::ourLanguage;
std::string ZLibrary::ourZLibraryDirectory;

std::string ZLibrary::ourApplicationName;
std::string ZLibrary::ourImageDirectory;
std::string ZLibrary::ourApplicationImageDirectory;
std::string ZLibrary::ourApplicationDirectory;
std::string ZLibrary::ourDefaultFilesPathPrefix;

const std::string ZLibrary::BaseDirectory = std::string(BASEDIR);

void ZLibrary::parseArguments(int &argc, char **&argv) {
	while ((argc > 2) && (argv[1] != 0) && (argv[2] != 0)) {
		static const std::string LANGUAGE_OPTION = "-lang";
		if (LANGUAGE_OPTION == argv[1]) {
			ourLanguage = argv[2];
		} else {
			break;
		}
		argc -= 2;
		argv += 2;
	}
	ourZLibraryDirectory = BaseDirectory + FileNameDelimiter + "zlibrary";
}

void ZLibrary::shutdown() {
	ZLImageManager::deleteInstance();
	ZLCommunicationManager::deleteInstance();
	ZLDialogManager::deleteInstance();
	ZLFSManager::deleteInstance();
	ZLTimeManager::deleteInstance();
	ZLConfigManager::deleteInstance();
}

std::string ZLibrary::replaceRegExps(const std::string &pattern) {
	static const std::string NAME_PATTERN = "%APPLICATION_NAME%";
	static const std::string LOWERCASENAME_PATTERN = "%application_name%";
	std::string str = pattern;
	int index = -1;
	while ((index = str.find(NAME_PATTERN)) != -1) {
	  str.erase(index, NAME_PATTERN.length());
		str.insert(index, ourApplicationName);
	}
	while ((index = str.find(LOWERCASENAME_PATTERN)) != -1) {
	  str.erase(index, LOWERCASENAME_PATTERN.length());
		str.insert(index, ZLUnicodeUtil::toLower(ourApplicationName));
	}
	return str;
}

void ZLibrary::initApplication(const std::string &name) {
	ourApplicationName = name;
	ourImageDirectory = replaceRegExps(IMAGEDIR);
	ourApplicationImageDirectory = replaceRegExps(APPIMAGEDIR);
	ourApplicationDirectory = BaseDirectory + FileNameDelimiter + ourApplicationName;
	ourDefaultFilesPathPrefix = ourApplicationDirectory + FileNameDelimiter + "default" + FileNameDelimiter;
}
