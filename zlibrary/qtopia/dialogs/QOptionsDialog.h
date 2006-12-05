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

#ifndef __QOPTIONSDIALOG_H__
#define __QOPTIONSDIALOG_H__

#include <vector>

#include <qwidget.h>
#include <qtabwidget.h>
#include <qlist.h>

#include "../../abstract/dialogs/ZLOptionsDialog.h"

#include "FullScreenDialog.h"

class QOptionView;
class QGridLayout;

class QOptionsDialogTab : public QWidget, public ZLDialogContent {
	Q_OBJECT

public:
	QOptionsDialogTab(QWidget *parent);
	~QOptionsDialogTab();

	void addOption(ZLOptionEntry *option);
	void addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1);

	void addItem(QWidget *widget, int row, int fromColumn, int toColumn);

	void close();
	void accept();

	QWidget *parentWidget();

private:
	void createViewByEntry(ZLOptionEntry *option, int fromColumn, int toColumn);

private:
	QGridLayout *myLayout;
	int myRowCounter;
	QWidget *myParentWidget;

	QList<QOptionView> myViews;
};

class QOptionsDialog : public FullScreenDialog, public ZLOptionsDialog {
	Q_OBJECT

public:
	QOptionsDialog(const std::string &id, const std::string &caption);
	~QOptionsDialog() {}
	ZLDialogContent &createTab(const std::string &name);

protected:
	const std::string &selectedTabName() const;
	void selectTab(const std::string &name);
	bool run();

	void resizeEvent(QResizeEvent *);
	void keyPressEvent(QKeyEvent *event);

private slots:
	void accept();

private:
	QTabWidget *myTabWidget;

	QList<QOptionsDialogTab> myTabs;
	std::vector<std::string> myTabNames;
};

class MyQTabWidget : public QTabWidget {
	Q_OBJECT

public:
	MyQTabWidget(QWidget *parent);
	void resizeEvent(QResizeEvent *event);

signals:
	void resized(const QSize &size);
};

#endif /* __QOPTIONSDIALOG_H__ */
