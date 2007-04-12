/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLQTDIALOGCONTENT_H__
#define __ZLQTDIALOGCONTENT_H__

#include <ZLDialogContent.h>

#include "ZLFullScreenDialog.h"

class QWidget;
class QGridLayout;

class ZLQtDialogContent : public ZLDialogContent {

public:
	ZLQtDialogContent(QWidget *parent, const std::string &name = "");
	~ZLQtDialogContent();

	void addOption(ZLOptionEntry *option);
	void addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1);

	void addItem(QWidget *widget, int row, int fromColumn, int toColumn);

	void close();

	QWidget *widget();
	QWidget *parentWidget();

private:
	void createViewByEntry(ZLOptionEntry *option, int fromColumn, int toColumn);

private:
	QWidget *myWidget;
	QGridLayout *myLayout;
	int myRowCounter;
	QWidget *myParentWidget;
};

#endif /* __ZLQTDIALOGCONTENT_H__ */
