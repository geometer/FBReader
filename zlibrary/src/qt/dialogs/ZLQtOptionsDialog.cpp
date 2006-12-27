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

#include <qlayout.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogContent.h"

MyQTabWidget::MyQTabWidget(QWidget *parent) : QTabWidget(parent) {
}

void MyQTabWidget::resizeEvent(QResizeEvent *event) {
	QTabWidget::resizeEvent(event);
	emit resized(event->size());
}

ZLQtOptionsDialog::ZLQtOptionsDialog(const std::string &id, const std::string &caption) : QDialog(0, caption.c_str(), true), ZLDesktopOptionsDialog(id) {
	QVBoxLayout *layout = new QVBoxLayout(this);

	myTabWidget = new MyQTabWidget(this);
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

ZLDialogContent &ZLQtOptionsDialog::createTab(const std::string &name) {
	ZLQtDialogContent *tab = new ZLQtDialogContent(myTabWidget);
	myTabWidget->insertTab(tab, name.c_str());
	myTabs.push_back(tab);
	myTabNames.push_back(name);
	return *tab;
}

const std::string &ZLQtOptionsDialog::selectedTabName() const {
	return myTabNames[myTabWidget->currentPageIndex()];
}

void ZLQtOptionsDialog::selectTab(const std::string &name) {
	std::vector<std::string>::const_iterator it = std::find(myTabNames.begin(), myTabNames.end(), name);
	if (it != myTabNames.end()) {
		myTabWidget->setCurrentPage(it - myTabNames.begin());
	}
}

bool ZLQtOptionsDialog::run() {
	for (std::vector<ZLQtDialogContent*>::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		(*it)->close();
	}
	return exec() == QDialog::Accepted;
}

void ZLQtOptionsDialog::accept() {
	for (std::vector<ZLQtDialogContent*>::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		(*it)->accept();
	}
	QDialog::accept();
}

void ZLQtOptionsDialog::resizeEvent(QResizeEvent *) {
  myTabWidget->resize(size());
}
