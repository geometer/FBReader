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

#ifndef __GTKOPTIONSDIALOG_H__
#define __GTKOPTIONSDIALOG_H__

#include <vector>

#include <gtk/gtkdialog.h>
#include <gtk/gtknotebook.h>
#include <gtk/gtktable.h>

#include <desktop/ZLDesktopOptionsDialog.h>

class GtkOptionView;

class GtkOptionsDialogTab : public ZLOptionsDialogTab {

public:
	GtkOptionsDialogTab();
	~GtkOptionsDialogTab();

	void addOption(ZLOptionEntry *option);
	void addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1);

	void accept();

	GtkWidget *widget() { return GTK_WIDGET (myTable); }

	void addItem (GtkWidget *what, int row, int fromColumn, int toColumn);
private:
	int addRow (void);
	GtkOptionView *viewByEntry(ZLOptionEntry *option, int row, int fromColumn, int toColumn);
private:
	GtkTable *myTable;
	gint myRowCounter;
	std::vector<GtkOptionView *> myViews;
};

class GtkOptionsDialog : public ZLDesktopOptionsDialog {

public:
	GtkOptionsDialog(const char *caption);
	~GtkOptionsDialog();
	ZLOptionsDialogTab *createTab(const std::string &name);

protected:
	std::string selectedTabName();
	void selectTab(const std::string &name);
	int run();

	void setSize(int width, int height);
	int width() const;
	int height() const;

private:
	GtkDialog *myDialog;
	GtkNotebook *myNotebook;
	std::vector<GtkOptionsDialogTab *> myTabs;
	std::vector<std::string> myTabNames;
};

// vim:ts=2:sw=2:noet

#endif /* __GTKOPTIONSDIALOG_H__ */
