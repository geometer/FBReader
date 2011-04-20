/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
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

#include "ZLDesktopOpenFileDialog.h"

static const int INVALID_VALUE = -111111;
static const std::string GROUP_NAME = "OpenFileDialog";

ZLDesktopOpenFileDialog::ZLDesktopOpenFileDialog() :
	myXOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP_NAME, "X", INVALID_VALUE),
	myYOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP_NAME, "Y", INVALID_VALUE),
	myWidthOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP_NAME, "Width", INVALID_VALUE),
	myHeightOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP_NAME, "Height", INVALID_VALUE) {
}

bool ZLDesktopOpenFileDialog::run() {
	if (myXOption.value() != INVALID_VALUE &&
			myYOption.value() != INVALID_VALUE) {
		setPosition(myXOption.value(), myYOption.value());
	}
	if (myWidthOption.value() != INVALID_VALUE &&
			myHeightOption.value() != INVALID_VALUE) {
		setSize(myWidthOption.value(), myHeightOption.value());
	}
	bool result = runInternal();
	myWidthOption.setValue(width());
	myHeightOption.setValue(height());
	myXOption.setValue(x());
	myYOption.setValue(y());
	return result;
}
