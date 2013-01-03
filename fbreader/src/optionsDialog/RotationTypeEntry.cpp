/*
 * Copyright (C) 2010-2013 Geometer Plus <contact@geometerplus.com>
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

#include <ZLResource.h>
#include <ZLView.h>

#include "RotationTypeEntry.h"

RotationTypeEntry::RotationTypeEntry(const ZLResource &resource, ZLIntegerOption &angleOption) : myAngleOption(angleOption) {
	myValues.push_back(resource["disabled"].value());
	myValues.push_back(resource["counterclockwise"].value());
	myValues.push_back(resource["180"].value());
	myValues.push_back(resource["clockwise"].value());
	myValues.push_back(resource["cycle"].value());
}

const std::string &RotationTypeEntry::initialValue() const {
	switch (myAngleOption.value()) {
		default:
			return myValues[0];
		case ZLView::DEGREES90:
			return myValues[1];
		case ZLView::DEGREES180:
			return myValues[2];
		case ZLView::DEGREES270:
			return myValues[3];
		case -1:
			return myValues[4];
	}
}

const std::vector<std::string> &RotationTypeEntry::values() const {
	return myValues;
}

void RotationTypeEntry::onAccept(const std::string &value) {
	int angle = ZLView::DEGREES0;
	if (value == myValues[1]) {
		angle = ZLView::DEGREES90;
	} else if (value == myValues[2]) {
		angle = ZLView::DEGREES180;
	} else if (value == myValues[3]) {
		angle = ZLView::DEGREES270;
	} else if (value == myValues[4]) {
		angle = -1;
	}
	myAngleOption.setValue(angle);
}
