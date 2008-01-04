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

#ifndef __ZLGTKDIALOGCONTENT_H__
#define __ZLGTKDIALOGCONTENT_H__

#include <map>

#include <gtk/gtktable.h>

#include <ZLDialogContent.h>

#include "../optionView/ZLGtkOptionViewHolder.h"

class ZLGtkDialogContent : public ZLDialogContent, public ZLGtkOptionViewHolder {

public:
	ZLGtkDialogContent(const ZLResource &resource);
	~ZLGtkDialogContent();

	void addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option);
	void addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0,
									const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1);

	GtkWidget *widget() { return GTK_WIDGET(myTable); }

private:
	int addRow();
	void createViewByEntry(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option, int row, int fromColumn, int toColumn);

	void attachWidget(ZLOptionView &view, GtkWidget *widget);
	void attachWidgets(ZLOptionView &view, GtkWidget *widget0, GtkWidget *widget1);
	void attachWidget(GtkWidget *what, int row, int fromColumn, int toColumn);

private:
	GtkTable *myTable;
	gint myRowCounter;

	struct Position {
		Position(int row, int fromColumn, int toColumn);
		int Row;
		int FromColumn;
		int ToColumn;
	};
	std::map<ZLOptionView*,Position> myOptionPositions;
};

inline ZLGtkDialogContent::Position::Position(int row, int fromColumn, int toColumn) : Row(row), FromColumn(fromColumn), ToColumn(toColumn) {}

#endif /* __ZLGTKDIALOGCONTENT_H__ */
