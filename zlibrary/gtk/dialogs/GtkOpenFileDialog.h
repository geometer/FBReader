/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __GTKOPENFILEDIALOG_H__
#define __GTKOPENFILEDIALOG_H__

#include <gtk/gtk.h>

#include <map>

#include <abstract/ZLOpenFileDialog.h>

class GtkOpenFileDialog : public ZLOpenFileDialog {
public:
	GtkOpenFileDialog(const char *caption, const ZLFileHandler &handler); 
	~GtkOpenFileDialog(); 

	void run();

	void activatedSlot(GtkTreePath *path, GtkTreeViewColumn *column);

private:
	GdkPixbuf *getPixmap(const std::string &fileName, bool dir);

	void updateListView(const std::string &selected);

private:
	GtkDialog *myDialog;
	GtkListStore *myStore;
	GtkTreeView *myView;
	GtkEntry *myCurrentDirectoryName;

	std::map<std::string,GdkPixbuf*> myPixmaps;
};

#endif

// vim:ts=2:sw=2:noet
