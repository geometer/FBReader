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

#ifndef __ZLGTKSELECTIONDIALOG_H__
#define __ZLGTKSELECTIONDIALOG_H__

#include <gtk/gtk.h>

#include <vector>
#include <map>

#include <ZLSelectionDialog.h>

class ZLGtkSelectionDialog : public ZLSelectionDialog {

public:
	ZLGtkSelectionDialog(const char *caption, ZLTreeHandler &handler); 
	~ZLGtkSelectionDialog(); 

	bool run();

	void activatedSlot();

protected:
	void exitDialog();
	void updateStateLine();
	void updateList();
	void selectItem(int index);

private:
	GdkPixbuf *getPixmap(const ZLTreeNodePtr node);

private:
	bool myExitFlag;
	bool myNodeSelected;
	GtkDialog *myDialog;
	GtkListStore *myStore;
	GtkTreeView *myView;
	GtkEntry *myStateLine;

	std::map<std::string,GdkPixbuf*> myPixmaps;
};

#endif /* __ZLGTKSELECTIONDIALOG_H__ */
