/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLFSManager.h"
#include "ZLDir.h"

ZLDir::ZLDir(const std::string &name) : myName(name) {
	ZLFSManager::instance().normalize(myName);
}

ZLDir::~ZLDir() {
}

const std::string &ZLDir::name() const {
	return myName;
}

std::string ZLDir::parentName() const {
	if (myName == "/") {
		return myName;
	}
	int index = myName.rfind(':');
	if (index == -1) {
		index = myName.rfind('/');
	}
	return (index <= 0) ? "/" : myName.substr(0, index);
}

std::string ZLDir::shortName() const {
	int index = myName.rfind(':');
	if (index == -1) {
		index = myName.rfind('/');
	}
	return myName.substr(index + 1);
}

std::string ZLDir::itemName(const std::string &shortName) const {
	if (shortName == "..") {
		return parentName();
	}
	return (myName == "/") ? "/" + shortName : myName + delimiter() + shortName;
}
