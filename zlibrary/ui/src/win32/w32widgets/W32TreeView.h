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

#ifndef __W32TREEVIEW_H__
#define __W32TREEVIEW_H__

#include <vector>
#include <map>
#include <string>

#include <shared_ptr.h>
#include <ZLUnicodeUtil.h>

#include "W32Control.h"

class W32TreeViewItem {

public:
	W32TreeViewItem(const std::string &text, int iconIndex);

	const ZLUnicodeUtil::Ucs2String &text() const;
	int iconIndex() const;

private:
	ZLUnicodeUtil::Ucs2String myText;	
	int myIconIndex;
};

class W32TreeView : public W32Control {

public:
	static const std::string ITEM_DOUBLE_CLICKED_EVENT;
	static const std::string ITEM_SELECTED_EVENT;

public:
	W32TreeView(short iconSize);
	Size minimumSize() const;

	void clear();
	void insert(const std::string &itemName, HICON icon);
	void select(int index);
	int selectedIndex() const;

private:
	int controlNumber() const;
	void allocate(WORD *&p, short &id) const;
	int allocationSize() const;
	void init(HWND parent, W32ControlCollection *collection);
	void notificationCallback(LPARAM lParam);

	void showItem(W32TreeViewItem &item, int index);
	void addIconToList(HICON icon);

private:
	short myIconSize;
	std::vector<shared_ptr<W32TreeViewItem> > myItems;

	std::vector<HICON> myIcons;
	std::map<HICON,int> myIconToIndexMap;

	int mySelectedIndex;
};

#endif /* __W32TREEVIEW_H__ */
