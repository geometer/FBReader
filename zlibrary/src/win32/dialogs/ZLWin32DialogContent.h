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

#ifndef __ZLWIN32DIALOGCONTENT_H__
#define __ZLWIN32DIALOGCONTENT_H__

#include <windows.h>

#include <shared_ptr.h>

#include "../../desktop/dialogs/ZLDesktopOptionsDialog.h"

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

class ZLWin32DialogContent : public ZLDialogContent {

public:
	ZLWin32DialogContent();
	~ZLWin32DialogContent();

	void addOption(ZLOptionEntry *option);
	void addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1);

	//Win32Widget *widget() { return WIN32_WIDGET(myTable); }

	//void addItem(Win32Widget *what, int row, int fromColumn, int toColumn);

private:
	int addRow();
	void createViewByEntry(ZLOptionEntry *option, int row, int fromColumn, int toColumn);

private:
	shared_ptr<ZLWin32DialogPanel> myPanel;
};

#endif /* __ZLWIN32DIALOGCONTENT_H__ */
