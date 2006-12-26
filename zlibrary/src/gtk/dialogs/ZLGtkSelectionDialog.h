/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include "../../desktop/dialogs/ZLDesktopSelectionDialog.h"

class ZLGtkSelectionDialog : public ZLDesktopSelectionDialog {

public:
	ZLGtkSelectionDialog(const char *caption, ZLTreeHandler &handler); 
	~ZLGtkSelectionDialog(); 

	bool run();

	void activatedSlot();

protected:
	void setSize(int width, int height);
	int width() const;
	int height() const;

	void exitDialog();
	void update(const std::string &selectedNodeName);

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
	std::vector<ZLTreeNodePtr> myNodes;
};

#endif /* __ZLGTKSELECTIONDIALOG_H__ */
