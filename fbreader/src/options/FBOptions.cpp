/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include "FBOptions.h"

FBOptions* FBOptions::ourInstance = 0;

static const std::string OPTIONS = "Options";

ColorProfile& FBOptions::getColorProfile() {
	return getColorProfileName() == ColorProfile::NIGHT ? nightColorProfile : dayColorProfile;
}
const std::string& FBOptions::getColorProfileName() const {
	return ColorProfileOption.value();
}
void FBOptions::setColorProfileName(const std::string& name) {
	ColorProfileOption.setValue(name);
}

FBOptions::FBOptions() :
	LeftMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "LeftMargin", 0, 1000, 4),
	RightMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "RightMargin", 0, 1000, 4),
	TopMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "TopMargin", 0, 1000, 0),
	BottomMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "BottomMargin", 0, 1000, 4),
	ColorProfileOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "ColorProfile", ColorProfile::DAY),
	dayColorProfile(ColorProfile::DAY),
	nightColorProfile(ColorProfile::NIGHT) {
}

ZLColorOption& FBOptions::backgroundColorOption() {
	return *(getColorProfile().BackgroundColorOption);
}

ZLColorOption& FBOptions::regularTextColorOption() {
	return *(getColorProfile().RegularTextColorOption);
}

ZLColorOption &FBOptions::colorOption(const std::string &style) {
	return getColorProfile().colorOption(style);
}
