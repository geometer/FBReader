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

#include <windows.h>
#include <commctrl.h>

#include <ZLApplication.h>
#include <ZLFile.h>

#include "W32TreeView.h"
#include "../../../../core/src/win32/util/W32WCHARUtil.h"

const std::string W32TreeView::ITEM_SELECTED_EVENT = "Tree View: Item Selected";
const std::string W32TreeView::ITEM_DOUBLE_CLICKED_EVENT = "Tree View: Item Double-Clicked";

static const WCHAR CLASSNAME_TREEVIEW[] = WC_TREEVIEW;

W32TreeViewItem::W32TreeViewItem(const std::string &text, int iconIndex) : myIconIndex(iconIndex) {
	::createNTWCHARString(myText, text);
}

const ZLUnicodeUtil::Ucs2String &W32TreeViewItem::text() const {
	return myText;
}

int W32TreeViewItem::iconIndex() const {
	return myIconIndex;
}

W32TreeView::W32TreeView(short iconSize) : W32Control(WS_BORDER | WS_TABSTOP | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS | TVS_SINGLEEXPAND), myIconSize(iconSize), mySelectedIndex(-1) {
}

void W32TreeView::clear() {
	myItems.clear();
	if (myWindow != 0) {
		SendMessage(myWindow, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
	}
}

void W32TreeView::addIconToList(HICON icon) {
	HIMAGELIST imageList = (HIMAGELIST)SendMessage(myWindow, TVM_GETIMAGELIST, 0, 0);
	ImageList_AddIcon(imageList, icon);
}

void W32TreeView::insert(const std::string &itemName, HICON icon) {
	std::map<HICON,int>::const_iterator it = myIconToIndexMap.find(icon);
	int iconIndex;
	if (it != myIconToIndexMap.end()) {
		iconIndex = it->second;
	} else {
		myIcons.push_back(icon);
		iconIndex = myIconToIndexMap.size();
		myIconToIndexMap[icon] = iconIndex;
		if (myWindow != 0) {
			addIconToList(icon);
		}
	}

	shared_ptr<W32TreeViewItem> item = new W32TreeViewItem(itemName, iconIndex);
	myItems.push_back(item);
	if (myWindow != 0) {
		showItem(*item, myItems.size() - 1);
	}
}

void W32TreeView::showItem(W32TreeViewItem &item, int index) {
	TVINSERTSTRUCT tvItem;
	tvItem.hParent = TVI_ROOT;
	tvItem.hInsertAfter = TVI_LAST;
	tvItem.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_CHILDREN | TVIF_SELECTEDIMAGE;
	tvItem.item.pszText = (WCHAR*)::wchar(item.text());
	tvItem.item.cchTextMax = item.text().size();
	tvItem.item.iImage = item.iconIndex();
	tvItem.item.iSelectedImage = item.iconIndex();
	tvItem.item.cChildren = 0;
	tvItem.item.lParam = index;

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

W32Widget::Size W32TreeView::minimumSize() const {
	return Size(60, 36);
}

void W32TreeView::init(HWND parent, W32ControlCollection *collection) {
	W32Control::init(parent, collection);

	HIMAGELIST imageList = ImageList_Create(myIconSize, myIconSize, ILC_COLOR32, 0, 100);
	SendMessage(myWindow, TVM_SETIMAGELIST, 0, (LPARAM)imageList);

	for (std::vector<shared_ptr<W32TreeViewItem> >::iterator it = myItems.begin(); it != myItems.end(); ++it) {
		showItem(**it, it - myItems.begin());
	}
	for (std::vector<HICON>::iterator it = myIcons.begin(); it != myIcons.end(); ++it) {
		addIconToList(*it);
	}
}

void W32TreeView::notificationCallback(LPARAM lParam) {
	NMTREEVIEW &notification = *(NMTREEVIEW*)lParam;
	switch (notification.hdr.code) {
		case TVN_ITEMEXPANDING:
			break;
		case TVN_ITEMEXPANDED:
			break;
		case TVN_SELCHANGING:
			break;
		case TVN_SELCHANGED:
			mySelectedIndex = notification.itemNew.lParam;
			fireEvent(ITEM_SELECTED_EVENT);
			break;
		// TODO: symbolic name?
		case -3:
			fireEvent(ITEM_DOUBLE_CLICKED_EVENT);
			break;
		case TVN_DELETEITEM:
			break;
		default:
			break;
	}
}

void W32TreeView::select(int index) {
	if ((index >= 0) && (index < (int)myItems.size())) {
		mySelectedIndex = index;
		if (myWindow != 0) {
			HTREEITEM item = TreeView_GetChild(myWindow, TVI_ROOT);
			for (int i = 0; i < mySelectedIndex; ++i) {
				item = TreeView_GetNextSibling(myWindow, item);
			}
			TreeView_SelectItem(myWindow, item);
			TreeView_EnsureVisible(myWindow, item);
		}
	}
}

int W32TreeView::selectedIndex() const {
	return mySelectedIndex;
}
