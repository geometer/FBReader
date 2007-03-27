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

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QResizeEvent>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogContent.h"

ZLQtOptionsDialog::ZLQtOptionsDialog(const std::string &id, const std::string &caption) : QDialog(qApp->activeWindow()), ZLDesktopOptionsDialog(id) {
	setModal(true);
	setWindowTitle(caption.c_str());
	QVBoxLayout *layout = new QVBoxLayout(this);

	myTabWidget = new QTabWidget(this);
	layout->addWidget(myTabWidget);

	QWidget *group = new QWidget(this);
	layout->addWidget(group);
	QGridLayout *buttonLayout = new QGridLayout(group);
	buttonLayout->setColumnStretch(0, 3);
	buttonLayout->setColumnStretch(1, 0);
	buttonLayout->setColumnStretch(2, 1);
	buttonLayout->setColumnStretch(3, 0);
	buttonLayout->setColumnStretch(4, 3);

	QPushButton *okButton = new QPushButton(group);
	okButton->setText("OK");
	buttonLayout->addWidget(okButton, 0, 1);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(group);
	cancelButton->setText("Cancel");
	buttonLayout->addWidget(cancelButton, 0, 3);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	if (parent() == 0) {
		QDesktopWidget *desktop = qApp->desktop();
		if (desktop != 0) {
			move((desktop->width() - width()) / 2, (desktop->height() - height()) / 2);
		}
	}
}

ZLQtOptionsDialog::~ZLQtOptionsDialog() {
	for (std::vector<ZLQtDialogContent*>::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		delete *it;
	}
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const std::string &name) {
	ZLQtDialogContent *tab = new ZLQtDialogContent(new QWidget(myTabWidget));
	myTabWidget->insertTab(myTabs.size(), tab->widget(), name.c_str());
	myTabs.push_back(tab);
	myTabNames.push_back(name);
	return *tab;
}

const std::string &ZLQtOptionsDialog::selectedTabName() const {
	return myTabNames[myTabWidget->currentIndex()];
}

void ZLQtOptionsDialog::selectTab(const std::string &name) {
	std::vector<std::string>::const_iterator it = std::find(myTabNames.begin(), myTabNames.end(), name);
	if (it != myTabNames.end()) {
		myTabWidget->setCurrentIndex(it - myTabNames.begin());
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
