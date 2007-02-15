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

#include <iostream>

#include <windows.h>
#include <commctrl.h>

#include <ZLApplication.h>
#include <ZLFile.h>

#include "W32TreeView.h"
#include "W32WCHARUtil.h"

static const WCHAR CLASSNAME_TREEVIEW[] = WC_TREEVIEW;

static HBITMAP maskBitmap(HWND window, HBITMAP original, int iconSize) {
	HDC dc = GetDC(window);
	HDC srcDC = CreateCompatibleDC(dc);
	HDC dstDC = CreateCompatibleDC(dc);
	HBITMAP result = CreateCompatibleBitmap(dc, iconSize, iconSize);
	SelectObject(srcDC, original);
	SelectObject(dstDC, result);
	static const COLORREF gray = RGB(0xC0, 0xC0, 0xC0);
	static const COLORREF white = RGB(0xFF, 0xFF, 0xFF);
	static const COLORREF black = RGB(0x00, 0x00, 0x00);
	for (int i = 0; i < iconSize; ++i) {
		for (int j = 0; j < iconSize; ++j) {
			COLORREF pixel = GetPixel(srcDC, i, j);
			SetPixel(dstDC, i, j, (pixel == gray) ? white : black);
		}
	}
	DeleteDC(dstDC);
	DeleteDC(srcDC);
	ReleaseDC(window, dc);
	return result;
}

W32TreeViewItem::W32TreeViewItem(const std::string &text, int iconIndex) : myIconIndex(iconIndex) {
	::createNTWCHARString(myText, text);
}

const ZLUnicodeUtil::Ucs2String &W32TreeViewItem::text() const {
	return myText;
}

int W32TreeViewItem::iconIndex() const {
	return myIconIndex;
}

W32TreeView::W32TreeView(short iconSize) : W32Control(WS_BORDER | TVS_DISABLEDRAGDROP), myIconSize(iconSize) {
}

void W32TreeView::clear() {
	myItems.clear();
	if (myWindow != 0) {
		SendMessage(myWindow, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
	}
}

void W32TreeView::addBitmapToList(HBITMAP bitmap) {
	if (bitmap == 0) {
		HDC dc = GetDC(myWindow);
		bitmap = CreateCompatibleBitmap(dc, myIconSize, myIconSize);
		ReleaseDC(myWindow, dc);
	}
	HBITMAP mask = maskBitmap(myWindow, bitmap, myIconSize);
	HIMAGELIST imageList = (HIMAGELIST)SendMessage(myWindow, TVM_GETIMAGELIST, 0, 0);
	ImageList_Add(imageList, bitmap, mask);
}

void W32TreeView::insert(const std::string &itemName, HBITMAP icon) {
	std::map<HBITMAP,int>::const_iterator it = myBitmapToIndexMap.find(icon);
	int iconIndex;
	if (it != myBitmapToIndexMap.end()) {
		iconIndex = it->second;
	} else {
		myBitmaps.push_back(icon);
		iconIndex = myBitmapToIndexMap.size();
		myBitmapToIndexMap[icon] = iconIndex;
		if (myWindow != 0) {
			addBitmapToList(icon);
		}
	}

	shared_ptr<W32TreeViewItem> item = new W32TreeViewItem(itemName, iconIndex);
	myItems.push_back(item);
	if (myWindow != 0) {
		showItem(*item);
	}
}

void W32TreeView::showItem(W32TreeViewItem &item) {
	TVINSERTSTRUCT tvItem;
	tvItem.hParent = TVI_ROOT;
	tvItem.hInsertAfter = TVI_LAST;
	tvItem.item.mask = TVIF_TEXT | TVIF_IMAGE;
	//tvItem.item.hItem = 0;
	//tvItem.item.state = 0;
	//tvItem.item.stateMask = 0;
	tvItem.item.pszText = (WCHAR*)::wchar(item.text());
	tvItem.item.cchTextMax = item.text().size();
	tvItem.item.iImage = item.iconIndex();
	//tvItem.item.iSelectedImage = 0;
	//tvItem.item.cChildren = 0;
	//tvItem.item.lParam = 0;

	SendMessage(myWindow, TVM_INSERTITEM, 0, (LPARAM)&tvItem);
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
	mySize.Width = 60 * charDimension.Width;
	mySize.Height = 20 * charDimension.Height;
}

void W32TreeView::init(HWND parent, W32ControlCollection *collection) {
	W32Control::init(parent, collection);

	HIMAGELIST imageList = ImageList_Create(myIconSize, myIconSize, ILC_COLOR8 | ILC_MASK, 0, 100);
	SendMessage(myWindow, TVM_SETIMAGELIST, 0, (LPARAM)imageList);

	for (std::vector<shared_ptr<W32TreeViewItem> >::iterator it = myItems.begin(); it != myItems.end(); ++it) {
		showItem(**it);
	}
	for (std::vector<HBITMAP>::iterator it = myBitmaps.begin(); it != myBitmaps.end(); ++it) {
		addBitmapToList(*it);
	}
}

void W32TreeView::notificationCallback(LPARAM lParam) {
	NMTREEVIEW &notification = *(NMTREEVIEW*)lParam;
	switch (notification.hdr.code) {
		case TVN_SELCHANGING:
			//std::cerr << "selection changing\n";
			break;
		case TVN_SELCHANGED:
			//mySelectedIndex = ;
			break;
		case TVN_ITEMEXPANDING:
			//std::cerr << "item expanding\n";
			break;
		case TVN_DELETEITEM:
			//std::cerr << "delete item\n";
			break;
		default:
			//std::cerr << "code = " << (int)notification.hdr.code << "\n";
			break;
	}
}

void W32TreeView::select(int index) {
	mySelectedIndex = index;
	if (myWindow != 0) {
		//SendMessage(myWindow, TVM_SELECTITEM, TVGN_FIRSTVISIBLE, );
	}
}

int W32TreeView::selectedIndex() const {
	return mySelectedIndex;
}
