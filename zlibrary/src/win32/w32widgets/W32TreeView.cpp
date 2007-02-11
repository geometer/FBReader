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

#include <windows.h>
#include <commctrl.h>

#include "W32TreeView.h"
#include "W32WCHARUtil.h"

static const WCHAR CLASSNAME_TREEVIEW[] = WC_TREEVIEW;

W32TreeView::W32TreeView() : W32Control(0) {
}

int W32TreeView::controlNumber() const {
	return 1;
}

void W32TreeView::allocate(WORD *&p, short &id) const {
	WORD *start = p;
	*p++ = LOWORD(myStyle);
	*p++ = HIWORD(myStyle);
	*p++ = 0;
	*p++ = 0;
	*p++ = myX;
	*p++ = myY;
	*p++ = mySize.Width;
	*p++ = mySize.Height;
	*p++ = id++;

	static const int classNameLength = lstrlenW(CLASSNAME_TREEVIEW);
	memcpy(p, CLASSNAME_TREEVIEW, classNameLength * 2);
	p += classNameLength;
	*p++ = 0;

	//*p++ = 0xFFFF;
	//*p++ = classId();
	*p++ = 0;
	*p++ = 0;
	*p++ = 0;

	if ((p - start) % 2 == 1) {
		*p++ = 0;
	}
}

int W32TreeView::allocationSize() const {
	static const int classNameLength = lstrlenW(CLASSNAME_TREEVIEW);
	int size = classNameLength + 13;
	return size + size % 2;
}

void W32TreeView::setDimensions(Size charDimension) {
	mySize.Width = 20 * charDimension.Width;
	mySize.Height = 20 * charDimension.Height;
}

void W32TreeView::init(HWND parent, W32ControlCollection *collection) {
	W32Control::init(parent, collection);

	{
		TVINSERTSTRUCT item;
		item.hParent = TVI_ROOT;
		item.hInsertAfter = TVI_LAST;
		item.item.mask = TVIF_TEXT;
		//item.item.hItem = 0;
		//item.item.state = 0;
		//item.item.stateMask = 0;
		static ZLUnicodeUtil::Ucs2String text;
		item.item.pszText = (WCHAR*)::wchar(::createNTWCHARString(text, "Text Item 0"));
		item.item.cchTextMax = text.size();
		//item.item.iImage = 0;
		//item.item.iSelectedImage = 0;
		//item.item.cChildren = 0;
		//item.item.lParam = 0;

		SendMessage(myWindow, TVM_INSERTITEM, 0, (LPARAM)&item);
	}
	{
		TVINSERTSTRUCT item;
		item.hParent = TVI_ROOT;
		item.hInsertAfter = TVI_LAST;
		item.item.mask = TVIF_TEXT;
		static ZLUnicodeUtil::Ucs2String text;
		item.item.pszText = (WCHAR*)::wchar(::createNTWCHARString(text, "Text Item 1"));
		item.item.cchTextMax = text.size();
		SendMessage(myWindow, TVM_INSERTITEM, 0, (LPARAM)&item);
	}
	{
		TVINSERTSTRUCT item;
		item.hParent = TVI_ROOT;
		item.hInsertAfter = TVI_LAST;
		item.item.mask = TVIF_TEXT;
		static ZLUnicodeUtil::Ucs2String text;
		item.item.pszText = (WCHAR*)::wchar(::createNTWCHARString(text, "Text Item 2"));
		item.item.cchTextMax = text.size();
		SendMessage(myWindow, TVM_INSERTITEM, 0, (LPARAM)&item);
	}
	{
		TVINSERTSTRUCT item;
		item.hParent = TVI_ROOT;
		item.hInsertAfter = TVI_LAST;
		item.item.mask = TVIF_TEXT;
		static ZLUnicodeUtil::Ucs2String text;
		item.item.pszText = (WCHAR*)::wchar(::createNTWCHARString(text, "Text Item 3"));
		item.item.cchTextMax = text.size();
		SendMessage(myWindow, TVM_INSERTITEM, 0, (LPARAM)&item);
	}
	{
		TVINSERTSTRUCT item;
		item.hParent = TVI_ROOT;
		item.hInsertAfter = TVI_LAST;
		item.item.mask = TVIF_TEXT;
		static ZLUnicodeUtil::Ucs2String text;
		item.item.pszText = (WCHAR*)::wchar(::createNTWCHARString(text, "Text Item 4"));
		item.item.cchTextMax = text.size();
		SendMessage(myWindow, TVM_INSERTITEM, 0, (LPARAM)&item);
	}
}
