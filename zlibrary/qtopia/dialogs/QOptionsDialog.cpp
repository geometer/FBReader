/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <algorithm>

#include <qtabwidget.h>
#include <qhbox.h>
#include <qvbox.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>

#include "../../abstract/dialogs/ZLOptionEntry.h"
#include "../../abstract/screenSize/ZLScreenSize.h"

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
	for (QOptionsDialogTab *tab = myTabs.first(); tab != NULL; tab = myTabs.next()) {
		tab->close();
	}
	return exec();
}

void QOptionsDialog::accept() {
	for (QOptionsDialogTab *tab = myTabs.first(); tab != NULL; tab = myTabs.next()) {
		tab->accept();
	}
	FullScreenDialog::accept();
}

void QOptionsDialogTab::accept() {
	for (QOptionView *view = myViews.first(); view != NULL; view = myViews.next()) {
		view->onAccept();
	}
}

void QOptionsDialog::resizeEvent(QResizeEvent *) {
  myTabWidget->resize(size());
}

void QOptionsDialog::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Key_Return) {
		accept();
	}
}

void QOptionsDialogTab::close() {
	myLayout->setRowStretch(myRowCounter, 10);
}

QOptionsDialogTab::QOptionsDialogTab(QTabWidget *parent) : QWidget(parent) {
	int space = 10;
	switch (ZLScreenSize::getSize()) {
		case ZLScreenSize::SIZE_DESKTOP:
			space = 10;
			break;
		case ZLScreenSize::SIZE_640x480:
			space = 10;
			break;
		case ZLScreenSize::SIZE_240x320:
			space = 3;
			break;
	}
	myLayout = new QGridLayout(this, -1, 13, space, space);
	myRowCounter = 0;
}

QOptionsDialogTab::~QOptionsDialogTab() {
	for (QOptionView *view = myViews.first(); view != NULL; view = myViews.next()) {
		delete view;
	}
}

void QOptionsDialogTab::addOption(ZLOptionEntry *option) {
	QOptionView *view = viewByEntry(option, 0, 12);
	if (view != 0) {
		myViews.append(view);
	}
	myRowCounter++;
}

void QOptionsDialogTab::addOptions(ZLOptionEntry *option0, ZLOptionEntry *option1) {
	QOptionView *view0 = viewByEntry(option0, 0, 5);
	if (view0 != 0) {
		myViews.append(view0);
	}
	QOptionView *view1 = viewByEntry(option1, 7, 12);
	if (view1 != 0) {
		myViews.append(view1);
	}
	myRowCounter++;
}

void QOptionsDialogTab::addItem(QWidget *widget, int row, int fromColumn, int toColumn) {
	myLayout->addMultiCellWidget(widget, row, row, fromColumn, toColumn);
}

QOptionView *QOptionsDialogTab::viewByEntry(ZLOptionEntry *option, int fromColumn, int toColumn) {
	if (option == 0) {
		return 0;
	}
	switch (option->kind()) {
		case BOOLEAN:
			return new BooleanOptionView((ZLBooleanOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
		case STRING:
			return new StringOptionView((ZLStringOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
		case CHOICE:
			return new ChoiceOptionView((ZLChoiceOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
		case SPIN:
			return new SpinOptionView((ZLSpinOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
		case COMBO:
			return new ComboOptionView((ZLComboOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
		case COLOR:
			return new ColorOptionView((ZLColorOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
		case UNKNOWN:
			QOptionView* view = (QOptionView*)((ZLUserDefinedOptionEntry*)option)->createView();
			view->setPosition(this, myRowCounter, fromColumn, toColumn);
			if (option->isVisible()) {
				view->createItem();
			}
			return view;
	}
	return 0;
}
