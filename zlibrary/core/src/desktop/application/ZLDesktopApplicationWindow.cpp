/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include "ZLDesktopApplicationWindow.h"

static const std::string OPTIONS = "Options";

ZLDesktopApplicationWindow::ZLDesktopApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application),
	myXOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "XPosition", 0, 2000, 10),
	myYOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "YPosition", 0, 2000, 10),
	myWidthOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "Width", 10, 2000, 800),
	myHeightOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "Height", 10, 2000, 600),
	myWindowStateOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "WindowState", NORMAL) {
}
