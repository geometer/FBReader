/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005, 2006 Mikhail Sobolev <mss@mawhrin.net>
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

#include <ZLDialogManager.h>
#include <ZLUnicodeUtil.h>

#include "ZLApplication.h"

#include "../options/ZLOptions_internal.h"

class ConfigSaverRunnable : public ZLRunnable {

public:
	void run() {
		ZLOptions::deleteInstance();
	}
};

const std::string ZLApplicationBase::BaseDirectory = std::string(BASEDIR);
const std::string ZLApplicationBase::HomeDirectory = std::string(HOMEDIR);
const std::string ZLApplicationBase::FileNameDelimiter = std::string(FILENAMEDELIMITER);
const std::string ZLApplicationBase::PathDelimiter = std::string(PATHDELIMITER);
const std::string ZLApplicationBase::EndOfLine = std::string(EOLCHARS);

std::string ZLApplicationBase::ourApplicationName;
std::string ZLApplicationBase::ourImageDirectory;
std::string ZLApplicationBase::ourApplicationImageDirectory;
std::string ZLApplicationBase::ourZLibraryDirectory;
std::string ZLApplicationBase::ourApplicationDirectory;
std::string ZLApplicationBase::ourDefaultFilesPathPrefix;

std::string ZLApplicationBase::replaceRegExps(const std::string &pattern) {
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

ZLApplicationBase::ZLApplicationBase(const std::string &name) {
	ourApplicationName = name;
	ourImageDirectory = replaceRegExps(IMAGEDIR);
	ourApplicationImageDirectory = replaceRegExps(APPIMAGEDIR);
	ourZLibraryDirectory = BaseDirectory + FileNameDelimiter + "zlibrary";
	ourApplicationDirectory = BaseDirectory + FileNameDelimiter + ourApplicationName;
	ourDefaultFilesPathPrefix = ourApplicationDirectory + FileNameDelimiter + "default" + FileNameDelimiter;
	ZLOptions::createInstance();
}

ZLApplicationBase::~ZLApplicationBase() {
	ConfigSaverRunnable configSaver;
	ZLDialogManager::instance().wait(ZLResourceKey("savingConfig"), configSaver);
}
