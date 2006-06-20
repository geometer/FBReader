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

#include <algorithm>

#include <qtabwidget.h>
#include <qhbox.h>
#include <qvbox.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>

#include <abstract/ZLOptionEntry.h>
#include <abstract/ZLDeviceInfo.h>

#include "QOptionsDialog.h"
#include "QOptionView.h"

QOptionsDialog::QOptionsDialog(const std::string &id, const std::string &caption) : FullScreenDialog(caption.c_str()), ZLOptionsDialog(id) {
	myTabWidget = new QTabWidget(this);
}

ZLOptionsDialogTab *QOptionsDialog::createTab(const std::string &name) {
	QOptionsDialogTab *tab = new QOptionsDialogTab(myTabWidget);
	myTabWidget->insertTab(tab, name.c_str());
	myTabs.append(tab);
	myTabNames.push_back(name);
	return tab;
}

const std::string &QOptionsDialog::selectedTabName() const {
	return myTabNames[myTabWidget->currentPageIndex()];
}

void QOptionsDialog::selectTab(const std::string &name) {
	std::vector<std::string>::const_iterator it = std::find(myTabNames.begin(), myTabNames.end(), name);
	if (it != myTabNames.end()) {
		myTabWidget->setCurrentPage(it - myTabNames.begin());
	}
}

bool QOptionsDialog::run() {
	for (QOptionsDialogTab *tab = myTabs.first(); tab != 0; tab = myTabs.next()) {
		tab->close();
	}
	return exec();
}

void QOptionsDialog::accept() {
	for (QOptionsDialogTab *tab = myTabs.first(); tab != 0; tab = myTabs.next()) {
		tab->accept();
	}
	FullScreenDialog::accept();
}

void QOptionsDialogTab::accept() {
	for (QOptionView *view = myViews.first(); view != 0; view = myViews.next()) {
		view->onAccept();
	}
}

void QOptionsDialog::resizeEvent(QResizeEvent *) {
  myTabWidget->resize(size());
}

void QOptionsDialog::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Key_Return) {
		accept();
	} else {
		FullScreenDialog::keyPressEvent(event);
	}
}

void QOptionsDialogTab::close() {
	myLayout->setRowStretch(myRowCounter, 10);
}

QOptionsDialogTab::QOptionsDialogTab(QTabWidget *parent) : QWidget(parent) {
	int space = 10;
	switch (ZLDeviceInfo::screenSize()) {
		case ZLDeviceInfo::SIZE_640x480:
			space = 10;
			break;
		default:
			space = 3;
			break;
	}
	myLayout = new QGridLayout(this, -1, 13, space, space);
	myRowCounter = 0;
}

QOptionsDialogTab::~QOptionsDialogTab() {
	for (QOptionView *view = myViews.first(); view != 0; view = myViews.next()) {
		delete view;
	}
}

void QOptionsDialogTab::addOption(ZLOptionEntry *option) {
	createViewByEntry(option, 0, 12);
	++myRowCounter;
}

void QOptionsDialogTab::addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) {
	createViewByEntry(option0, 0, 5);
	createViewByEntry(option1, 7, 12);
	++myRowCounter;
}

void QOptionsDialogTab::addItem(QWidget *widget, int row, int fromColumn, int toColumn) {
	myLayout->addMultiCellWidget(widget, row, row, fromColumn, toColumn);
}

void QOptionsDialogTab::createViewByEntry(ZLOptionEntry *option, int fromColumn, int toColumn) {
	if (option == 0) {
		return;
	}

	QOptionView *view = 0;
	switch (option->kind()) {
		case BOOLEAN:
			view = new BooleanOptionView((ZLBooleanOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case STRING:
			view = new StringOptionView((ZLStringOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case CHOICE:
			view = new ChoiceOptionView((ZLChoiceOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case SPIN:
			view = new SpinOptionView((ZLSpinOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case COMBO:
			view = new ComboOptionView((ZLComboOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case COLOR:
			view = new ColorOptionView((ZLColorOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case KEY:
			view = new KeyOptionView((ZLKeyOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
			break;
		case UNKNOWN:
			view = (QOptionView*)((ZLUserDefinedOptionEntry*)option)->createView();
			view->setPosition(this, myRowCounter, fromColumn, toColumn);
			break;
	}

	if (view != 0) {
		view->setVisible(option->isVisible());
		myViews.append(view);
	}
}
