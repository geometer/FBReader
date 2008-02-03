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

#ifndef __ZLQTDIALOGCONTENT_H__
#define __ZLQTDIALOGCONTENT_H__

#include <map>

#include <ZLDialogContent.h>

#include "../optionView/ZLQtOptionViewHolder.h"

class QWidget;
class QGridLayout;

class ZLQtDialogContent : public ZLDialogContent, public ZLQtOptionViewHolder {

public:
	ZLQtDialogContent(QWidget *parent, const ZLResource &resource);
	~ZLQtDialogContent();

	void addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option);
	void addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0,
									const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1);

	void close();

	QWidget *widget();
	QWidget *parentWidget();

private:
	void attachWidget(QWidget *widget, int row, int fromColumn, int toColumn);
	void attachWidget(ZLOptionView &view, QWidget *widget);
	void attachWidgets(ZLOptionView &view, QWidget *widget0, int weight0, QWidget *widget1, int weight1);

	void createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, int fromColumn, int toColumn);

private:
	QWidget *myWidget;
	QGridLayout *myLayout;
	int myRowCounter;
	QWidget *myParentWidget;

	struct Position {
		Position(int row, int fromColumn, int toColumn);
		int Row;
		int FromColumn;
		int ToColumn;
	};
	std::map<ZLOptionView*,Position> myOptionPositions;
};

inline ZLQtDialogContent::Position::Position(int row, int fromColumn, int toColumn) : Row(row), FromColumn(fromColumn), ToColumn(toColumn) {}

#endif /* __ZLQTDIALOGCONTENT_H__ */
