/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLWIN32DIALOGCONTROL_H__
#define __ZLWIN32DIALOGCONTROL_H__

#include <vector>
#include <string>

#include <windows.h>

#include <shared_ptr.h>

class ZLWin32DialogControl {

private:
	static int allocateString(WORD *p, const std::string &text);

public:
	ZLWin32DialogControl(DWORD style, int x, int y, int width, int height, WORD id, const std::string &className, const std::string &text);

private:
	void allocate(WORD *p) const;
	int allocationSize() const;

private:
	DWORD myStyle;
	int myX, myY;
	int myWidth, myHeight;
	WORD myId;
	std::string myClassName;
	std::string myText;

friend class ZLWin32DialogPanel;

private:
	ZLWin32DialogControl(const ZLWin32DialogControl&);
	const ZLWin32DialogControl &operator = (const ZLWin32DialogControl&);
};

class ZLWin32DialogPanel {

public:
	ZLWin32DialogPanel(DWORD style, int x, int y, int width, int height, const std::string &text);
	~ZLWin32DialogPanel();
	DLGTEMPLATE *dialogTemplate() const;
	void addControl(shared_ptr<ZLWin32DialogControl> control);

private:
	int allocationSize() const;

private:
	DWORD myStyle;
	int myX, myY;
	int myWidth, myHeight;
	std::string myText;

	typedef std::vector<shared_ptr<ZLWin32DialogControl> > ControlList;
	ControlList myControls;

	mutable WORD *myAddress;

private:
	ZLWin32DialogPanel(const ZLWin32DialogPanel&);
	const ZLWin32DialogPanel &operator = (const ZLWin32DialogPanel&);
};

#endif /* __ZLWIN32DIALOGCONTROL_H__ */
