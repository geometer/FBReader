/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLDesktopOpenFileDialog.h"

static const std::string OPTION_GROUP_NAME = "OpenFileDialog";

ZLDesktopOpenFileDialog::ZLDesktopOpenFileDialog(const ZLTreeHandler &handler) : ZLOpenFileDialog(handler), WidthOption(OPTION_GROUP_NAME, "Width", 10, 2000, 400), HeightOption(OPTION_GROUP_NAME, "Height", 10, 2000, 300) {
}

ZLDesktopOpenFileDialog::~ZLDesktopOpenFileDialog() {
}

void ZLDesktopOpenFileDialog::runWithSize() {
	setSize(WidthOption.value(), HeightOption.value());
	run();
	WidthOption.setValue(width());
	HeightOption.setValue(height());
}
