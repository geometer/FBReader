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

#ifndef __QOPTIONSDIALOG_H__
#define __QOPTIONSDIALOG_H__

#include <vector>

#include <qwidget.h>
#include <qdialog.h>
#include <qptrlist.h>

#include <desktop/ZLDesktopOptionsDialog.h>

class QTabWidget;
class QOptionView;
class QGridLayout;

class QOptionsDialogTab : public QWidget, public ZLOptionsDialogTab {
	Q_OBJECT

public:
	QOptionsDialogTab(QTabWidget *parent);
	~QOptionsDialogTab();

	void addOption(ZLOptionEntry *option);
	void addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1);

	void addItem(QWidget *widget, int row, int fromColumn, int toColumn);

	void close();
	void accept();

private:
	QOptionView *viewByEntry(ZLOptionEntry *option, int fromColumn, int toColumn);

private:
	QGridLayout *myLayout;
	int myRowCounter;

	QPtrList<QOptionView> myViews;
};

class QOptionsDialog : public QDialog, public ZLDesktopOptionsDialog {
	Q_OBJECT

public:
	QOptionsDialog(const char *caption);
	~QOptionsDialog() {}
	ZLOptionsDialogTab *createTab(const std::string &name);

protected:
	std::string selectedTabName();
	void selectTab(const std::string &name);
	int run();

	void setSize(int width, int height) { QDialog::resize(width, height); }
	int width() const { return QDialog::width(); }
	int height() const { return QDialog::height(); }

private slots:
	void resizeEvent(QResizeEvent *);
	void accept();

private:
	QTabWidget *myTabWidget;

	QList<QOptionsDialogTab> myTabs;
	std::vector<std::string> myTabNames;
};

#endif /* __QOPTIONSDIALOG_H__ */
