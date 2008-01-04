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

#ifndef __ZLWIN32SELECTIONDIALOG_H__
#define __ZLWIN32SELECTIONDIALOG_H__

#include <vector>
#include <map>

#include <windows.h>

#include "../../../../core/src/desktop/dialogs/ZLDesktopSelectionDialog.h"

#include "../w32widgets/W32DialogPanel.h"
#include "../w32widgets/W32TreeView.h"
#include "../w32widgets/W32Control.h"

class ZLWin32ApplicationWindow;

class ZLWin32SelectionDialog : public ZLDesktopSelectionDialog, public W32Listener {

public:
	ZLWin32SelectionDialog(ZLWin32ApplicationWindow &window, const std::string &caption, ZLTreeHandler &handler); 
	~ZLWin32SelectionDialog(); 

	bool run();

	//void activatedSlot();

protected:
	void setSize(int width, int height);
	int width() const;
	int height() const;

	void exitDialog();
	void updateStateLine();
	void updateList();
	void selectItem(int index);

private:
	void onEvent(const std::string &event, W32EventSender &sender);

private:
	HICON getIcon(const ZLTreeNodePtr node);

private:
	ZLWin32ApplicationWindow &myWindow;
	W32StandaloneDialogPanel myPanel;
	W32TreeView *myTreeView;
	W32LineEditor *myLineEditor;
	W32PushButton *myOkButton;

	std::map<std::string,HICON> myIcons;

	short myWidth;
	short myHeight;
};

#endif /* __ZLWIN32SELECTIONDIALOG_H__ */
