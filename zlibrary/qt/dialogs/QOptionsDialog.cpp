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

#include <algorithm>

#include <qtabwidget.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>

#include "../../abstract/dialogs/ZLOptionEntry.h"

#include "QOptionsDialog.h"
#include "QOptionView.h"

QOptionsDialog::QOptionsDialog(const char *caption) : QDialog(0, caption, true) {
	QVBoxLayout *layout = new QVBoxLayout(this);

	myTabWidget = new QTabWidget(this);
	layout->add(myTabWidget);

	QButtonGroup *group = new QButtonGroup(this);
	layout->add(group);
	QGridLayout *buttonLayout = new QGridLayout(group, 1, 5, 8, 0);
	buttonLayout->setColStretch(0, 3);
	buttonLayout->setColStretch(1, 0);
	buttonLayout->setColStretch(2, 1);
	buttonLayout->setColStretch(3, 0);
	buttonLayout->setColStretch(4, 3);

	QPushButton *okButton = new QPushButton(group);
	okButton->setText("OK");
	buttonLayout->addWidget(okButton, 0, 1);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(group);
	cancelButton->setText("Cancel");
	buttonLayout->addWidget(cancelButton, 0, 3);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

ZLOptionsDialogTab *QOptionsDialog::createTab(const std::string &name) {
	QOptionsDialogTab *tab = new QOptionsDialogTab(myTabWidget);
	myTabWidget->insertTab(tab, name.c_str());
	myTabs.append(tab);
	myTabNames.push_back(name);
	return tab;
}

std::string QOptionsDialog::selectedTabName() {
	return myTabNames[myTabWidget->currentPageIndex()];
}

void QOptionsDialog::selectTab(const std::string &name) {
	std::vector<std::string>::const_iterator it = std::find(myTabNames.begin(), myTabNames.end(), name);
	if (it != myTabNames.end()) {
		myTabWidget->setCurrentPage(it - myTabNames.begin());
	}
}

int QOptionsDialog::run() {
	for (QOptionsDialogTab *tab = myTabs.first(); tab != NULL; tab = myTabs.next()) {
		tab->close();
	}
	return exec();
}

void QOptionsDialog::accept() {
	for (QOptionsDialogTab *tab = myTabs.first(); tab != NULL; tab = myTabs.next()) {
		tab->accept();
	}
	QDialog::accept();
}

void QOptionsDialogTab::accept() {
	for (QOptionView *view = myViews.first(); view != NULL; view = myViews.next()) {
		view->onAccept();
	}
}

void QOptionsDialog::resizeEvent(QResizeEvent *) {
  myTabWidget->resize(size());
}

void QOptionsDialogTab::close() {
	myLayout->setRowStretch(myRowCounter, 10);
}

QOptionsDialogTab::QOptionsDialogTab(QTabWidget *parent) : QWidget(parent) {
	myLayout = new QGridLayout(this, -1, 13, 12, 12);
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
