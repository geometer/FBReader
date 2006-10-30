/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "../xmlconfig/XMLOptions.h"

const std::string ZLApplicationBase::BaseDirectory = std::string(BASEDIR);
const std::string ZLApplicationBase::HomeDirectory = std::string(HOMEDIR);
const std::string ZLApplicationBase::PathDelimiter = "/";

std::string ZLApplicationBase::ourApplicationName;
std::string ZLApplicationBase::ourImageDirectory;
std::string ZLApplicationBase::ourApplicationSubdirectoryName;

class ConfigSaverRunnable : public ZLRunnable {

public:
	void run() {
		XMLOptions::deleteInstance();
	}
};

void ZLApplicationBase::replaceRegExps(std::string &str) {
	static const std::string NAME_PATTERN = "%APPLICATION_NAME%";
	static const std::string LOWERCASENAME_PATTERN = "%application_name%";
	int index = -1;
	while ((index = str.find(NAME_PATTERN)) != -1) {
	  str.erase(index, NAME_PATTERN.length());
		str.insert(index, ourApplicationName);
	}
	while ((index = str.find(LOWERCASENAME_PATTERN)) != -1) {
	  str.erase(index, LOWERCASENAME_PATTERN.length());
		str.insert(index, ZLUnicodeUtil::toLower(ourApplicationName));
	}
}

ZLApplicationBase::ZLApplicationBase(const std::string &name) {
	ourApplicationName = name;
	ourImageDirectory = IMAGEDIR;
	replaceRegExps(ourImageDirectory);
	ourApplicationSubdirectoryName = APPLICATIONSUBDIR;
	replaceRegExps(ourApplicationSubdirectoryName);
	XMLOptions::createInstance();
}

ZLApplicationBase::~ZLApplicationBase() {
	ConfigSaverRunnable configSaver;
	ZLDialogManager::instance().wait(configSaver, "Saving config...");
}

const std::string ZLApplicationBase::ApplicationDirectory() {
	return BaseDirectory + PathDelimiter + ourApplicationName;
}

const std::string ZLApplicationBase::ZLibraryDirectory() {
	return BaseDirectory + PathDelimiter + "zlibrary";
}

const std::string ZLApplicationBase::DefaultFilesPathPrefix() {
	return ApplicationDirectory() + PathDelimiter + "default" + PathDelimiter;
}
