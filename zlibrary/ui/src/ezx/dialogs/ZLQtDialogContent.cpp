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

#include <zapplication.h>
#include <qlayout.h>

#include <ZLOptionEntry.h>

#include "ZLQtDialogContent.h"
#include "../../../../core/src/dialogs/ZLOptionView.h"

void ZLQtDialogContent::close() {
	myLayout->setRowStretch(myRowCounter, 10);
}

ZLQtDialogContent::ZLQtDialogContent(QWidget *parent, const ZLResource &resource) : ZLDialogContent(resource), myParentWidget(parent) {
	myWidget = new QWidget(myParentWidget);
	const long displaySize = qApp->desktop()->height() * (long)qApp->desktop()->width();
	const int space = (displaySize < 640 * 480) ? 3 : 10;
	myLayout = new QGridLayout(myWidget, -1, 13, space, space);
	myRowCounter = 0;
}

ZLQtDialogContent::~ZLQtDialogContent() {
}

void ZLQtDialogContent::addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option) {
	createViewByEntry(name, tooltip, option, 0, 12);
	++myRowCounter;
}

void ZLQtDialogContent::addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0, const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1) {
	createViewByEntry(name0, tooltip0, option0, 0, 5);
	createViewByEntry(name1, tooltip1, option1, 7, 12);
	++myRowCounter;
}

void ZLQtDialogContent::attachWidget(QWidget *widget, int row, int fromColumn, int toColumn) {
	myLayout->addMultiCellWidget(widget, row, row, fromColumn, toColumn);
}

void ZLQtDialogContent::attachWidget(ZLOptionView &view, QWidget *widget) {
	std::map<ZLOptionView*,Position>::const_iterator it = myOptionPositions.find(&view);
	if (it != myOptionPositions.end()) {
		Position position = it->second;
		attachWidget(widget, position.Row, position.FromColumn, position.ToColumn);
	}
}	

void ZLQtDialogContent::attachWidgets(ZLOptionView &view, QWidget *widget0, int weight0, QWidget *widget1, int weight1) {
	std::map<ZLOptionView*,Position>::const_iterator it = myOptionPositions.find(&view);
	if (it != myOptionPositions.end()) {
		Position position = it->second;
		int secondStart = position.FromColumn + (position.ToColumn - position.FromColumn + 1) * weight0 / (weight0 + weight1);
		attachWidget(widget0, position.Row, position.FromColumn, secondStart - 1);
		attachWidget(widget1, position.Row, secondStart, position.ToColumn);
	}
}	

void ZLQtDialogContent::createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, int fromColumn, int toColumn) {
	if (option == 0) {
		return;
	}

	ZLOptionView *view = ZLQtOptionViewHolder::createViewByEntry(name, tooltip, option);

	if (view != 0) {
		myOptionPositions.insert(
			std::pair<ZLOptionView*,Position>(view, Position(myRowCounter, fromColumn, toColumn))
		);
		addView(view);
		view->setVisible(option->isVisible());
	}
}

QWidget *ZLQtDialogContent::widget() {
	return myWidget;
}

QWidget *ZLQtDialogContent::parentWidget() {
	return myParentWidget;
}
