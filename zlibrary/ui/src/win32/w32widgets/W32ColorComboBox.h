/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __W32COLORCOMBOBOX_H__
#define __W32COLORCOMBOBOX_H__

#include <ZLColor.h>
#include <ZLUnicodeUtil.h>

#include "W32Control.h"

class W32ColorComboBox : public W32StandardControl {

private:
	static std::vector<ZLUnicodeUtil::Ucs2String> ourStrings;
	static std::vector<ZLColor> ourColors;

public:
	static const std::string SELECTION_CHANGED_EVENT;

public:
	W32ColorComboBox(ZLColor initialColor);
	Size minimumSize() const;

	WORD classId() const;
	void init(HWND parent, W32ControlCollection *collection);

	void commandCallback(DWORD hiWParam);
	void drawItemCallback(DRAWITEMSTRUCT &di);

	ZLColor selectedColor() const;
	void setSelectedColor(ZLColor color);

	void removeCustomColor();

private:
	static void addColor(const std::string &name, ZLColor color);
	static void initVectors();
	const ZLUnicodeUtil::Ucs2String &text(int index) const;
	const ZLColor color(int index) const;

private:
	bool myUseCustomColor;
	ZLColor myCustomColor;
	int myIndex;
	int myPixelWidth;
	int myPixelHeight;
};

#endif /* __W32COLORCOMBOBOX_H__ */
