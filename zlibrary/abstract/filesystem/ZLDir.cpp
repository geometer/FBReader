/*
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

#include "ZLDir.h"

std::string ZLDir::HomeDir = getenv("HOME");

ZLDir::ZLDir(const std::string &name) : myName(name) {
	if (myName.empty()) {
		myName = getenv("PWD");
	} else if (myName[0] == '~') {
		if ((myName.length() == 1) || (myName[1] == '/')) {
			myName = HomeDir + myName.substr(1);
		}
	} else if (myName[0] != '/') {
		myName = getenv("PWD") + '/' + myName;
	}
	int last = myName.length() - 1;
	while ((last > 0) && (myName[last] == '/')) {
		last--;
	}
	if (last < (int)myName.length() - 1) {
		myName = myName.substr(0, last + 1);
	}
}

std::string ZLDir::parentName() const {
	if (myName == "/") {
		return myName;
	}
	int index = myName.rfind('/');
	if (index <= 0) {
		return "/";
	}
	return myName.substr(0, index);
}

std::string ZLDir::shortName() const {
	return myName.substr(myName.rfind('/') + 1);
}

std::string ZLDir::itemName(const std::string &shortName) const {
	if (shortName == "..") {
		return parentName();
	}
	return (myName == "/") ? "/" + shortName : myName + delimiter() + shortName;
}
