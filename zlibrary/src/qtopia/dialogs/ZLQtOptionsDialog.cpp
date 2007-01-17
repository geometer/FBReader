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

#include <algorithm>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogManager.h"

MyQTabWidget::MyQTabWidget(QWidget *parent) : QTabWidget(parent) {
}

void MyQTabWidget::resizeEvent(QResizeEvent *event) {
	QTabWidget::resizeEvent(event);
	emit resized(event->size());
}

ZLQtOptionsDialog::ZLQtOptionsDialog(const std::string &id, const std::string &caption) : ZLFullScreenDialog(caption.c_str()), ZLOptionsDialog(id) {
	myTabWidget = new MyQTabWidget(this);
}

ZLQtOptionsDialog::~ZLQtOptionsDialog() {
	for (std::vector<ZLQtDialogContent*>::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		delete *it;
	}
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const std::string &name) {
	ZLQtDialogContent *tab = new ZLQtDialogContent(myTabWidget);
	myTabWidget->insertTab(tab->widget(), name.c_str());
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
	bool code = exec();
	((ZLQtDialogManager&)ZLQtDialogManager::instance()).fullScreenWorkaround();
	return code;
}

void ZLQtOptionsDialog::accept() {
	for (std::vector<ZLQtDialogContent*>::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		(*it)->accept();
	}
	ZLFullScreenDialog::accept();
}

void ZLQtOptionsDialog::resizeEvent(QResizeEvent *) {
  myTabWidget->resize(size());
}

void ZLQtOptionsDialog::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Key_Return) {
		accept();
	} else {
		ZLFullScreenDialog::keyPressEvent(event);
	}
}
