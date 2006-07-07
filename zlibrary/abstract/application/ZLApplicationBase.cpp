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

#include <abstract/ZLDialogManager.h>

#include "ZLApplication.h"

#include "../xmlconfig/XMLOptions.h"

std::string ZLApplicationBase::ourApplicationName;

class ConfigSaverRunnable : public ZLRunnable {

public:
	void run() {
		XMLOptions::deleteInstance();
	}
};

ZLApplicationBase::ZLApplicationBase(const std::string &name) {
	ourApplicationName = name;
	XMLOptions::createInstance();
}

ZLApplicationBase::~ZLApplicationBase() {
	ConfigSaverRunnable configSaver;
	ZLDialogManager::instance().wait(configSaver, "Saving config...");
}

const std::string &ZLApplicationBase::ApplicationName() {
	return ourApplicationName;
}

const std::string ZLApplicationBase::ApplicationDirectory() {
	return BaseDirectory + PathDelimiter + ourApplicationName;
}

const std::string ZLApplicationBase::ZLibraryDirectory() {
	return BaseDirectory + PathDelimiter + "zlibrary";
}
