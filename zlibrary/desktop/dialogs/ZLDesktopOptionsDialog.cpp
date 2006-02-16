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

#include "ZLDesktopOptionsDialog.h"

ZLDesktopOptionsDialog::ZLDesktopOptionsDialog(const std::string &id) : ZLOptionsDialog(id), WidthOption(id, "Width", 10, 2000, 400), HeightOption(id, "Height", 10, 2000, 300) {
}

bool ZLDesktopOptionsDialog::run(const std::string &tabName) {
	setSize(WidthOption.value(), HeightOption.value());
	bool code = ZLOptionsDialog::run(tabName);
	WidthOption.setValue(width());
	HeightOption.setValue(height());
	return code;
}
