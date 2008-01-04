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

#include <ZLOptionEntry.h>

#include "ZLGtkDialogContent.h"
#include "../../../../core/src/dialogs/ZLOptionView.h"

ZLGtkDialogContent::ZLGtkDialogContent(const ZLResource &resource) : ZLDialogContent(resource) {
	myTable = GTK_TABLE(gtk_table_new(0, 4, false));
	gtk_container_set_border_width(GTK_CONTAINER(myTable), 2);

	myRowCounter = 0;

	gtk_widget_show(GTK_WIDGET(myTable));
}

ZLGtkDialogContent::~ZLGtkDialogContent() {
	// We must not delete the widget, it's destroyed when the parent widget is destroyed
}

int ZLGtkDialogContent::addRow() {
	int row = myRowCounter++;

	gtk_table_resize(myTable, myRowCounter, 2);

	return row;
}

void ZLGtkDialogContent::addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option) {
	int row = addRow();

	createViewByEntry(name, tooltip, option, row, 0, 4);
}

void ZLGtkDialogContent::addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0, const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1) {
	int row = addRow();

	createViewByEntry(name0, tooltip0, option0, row, 0, 2);
	createViewByEntry(name1, tooltip1, option1, row, 2, 4);
}

void ZLGtkDialogContent::createViewByEntry(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, int row, int fromColumn, int toColumn) {
	ZLOptionView *view = ZLGtkOptionViewHolder::createViewByEntry(name, tooltip, option);
	if (view != 0) {
		myOptionPositions.insert(
			std::pair<ZLOptionView*,Position>(view, Position(row, fromColumn, toColumn))
		);
		view->setVisible(option->isVisible());
		addView(view);
	}
}

void ZLGtkDialogContent::attachWidget(GtkWidget *what, int row, int fromColumn, int toColumn) {
	gtk_table_attach(myTable, what, fromColumn, toColumn, row, row + 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_FILL, 2, 1);
}

void ZLGtkDialogContent::attachWidget(ZLOptionView &view, GtkWidget *widget) {
	std::map<ZLOptionView*,Position>::const_iterator it = myOptionPositions.find(&view);
	if (it != myOptionPositions.end()) {
		Position position = it->second;
		attachWidget(widget, position.Row, position.FromColumn, position.ToColumn);
	}
}

void ZLGtkDialogContent::attachWidgets(ZLOptionView &view, GtkWidget *widget0, GtkWidget *widget1) {
	std::map<ZLOptionView*,Position>::const_iterator it = myOptionPositions.find(&view);
	if (it != myOptionPositions.end()) {
		Position position = it->second;
		int midColumn = (position.FromColumn + position.ToColumn) / 2;
		attachWidget(widget0, position.Row, position.FromColumn, midColumn);
		attachWidget(widget1, position.Row, midColumn, position.ToColumn);
	}
}
