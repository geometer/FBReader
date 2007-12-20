/*
 * Copyright (C) 2004-2007 Geometer Plus <contact@geometerplus.com>
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

#include <ZLOptionsDialog.h>
#include <optionEntries/ZLSimpleOptionEntry.h>

#include "ZLMaemoTapDetectorInfo.h"

static const std::string GROUP = "StylusPressure";

ZLMaemoTapDetectorInfo::ZLMaemoTapDetectorInfo() :
	MinPressureOption(ZLOption::CONFIG_CATEGORY, GROUP, "Minimum", 0, 100, 0),
	MaxPressureOption(ZLOption::CONFIG_CATEGORY, GROUP, "Maximum", 0, 100, 40) {
}

void ZLMaemoTapDetectorInfo::buildTabs(ZLOptionsDialog &dialog) {
	ZLDialogContent &tab = dialog.createTab(ZLResourceKey("Tap"));
	tab.addOption(
		ZLResourceKey("minStylusPressure"),
		new ZLSimpleSpinOptionEntry(MinPressureOption, 1)
	);
	tab.addOption(
		ZLResourceKey("maxStylusPressure"),
		new ZLSimpleSpinOptionEntry(MaxPressureOption, 1)
	);
}
