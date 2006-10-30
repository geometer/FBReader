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

#include <qapplication.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>

#include <ZLOptionEntry.h>

#include "QOptionsDialog.h"
#include "QOptionView.h"

QOptionsDialog::QOptionsDialog(const std::string &id, const std::string &caption) : QDialog(0, caption.c_str(), true), ZLDesktopOptionsDialog(id) {
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

ZLDialogContent &QOptionsDialog::createTab(const std::string &name) {
	QOptionsDialogTab *tab = new QOptionsDialogTab(myTabWidget);
	myTabWidget->insertTab(tab, name.c_str());
	myTabs.append(tab);
	myTabNames.push_back(name);
	return *tab;
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
	return exec() == QDialog::Accepted;
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

QOptionsDialogTab::QOptionsDialogTab(QWidget *parent) : QWidget(parent) {
	const long displaySize = qApp->desktop()->height() * (long)qApp->desktop()->width();
	const int space = (displaySize < 640 * 480) ? 3 : 10;
	myLayout = new QGridLayout(this, -1, 13, space, space);
	myRowCounter = 0;
}

QOptionsDialogTab::~QOptionsDialogTab() {
	for (QOptionView *view = myViews.first(); view != NULL; view = myViews.next()) {
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
		case ORDER:
			view = new OrderOptionView((ZLOrderOptionEntry*)option, this, myRowCounter, fromColumn, toColumn);
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
