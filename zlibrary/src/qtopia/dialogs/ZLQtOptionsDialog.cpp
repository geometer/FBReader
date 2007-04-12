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
#include "ZLQtDialogContent.h"

MyQTabWidget::MyQTabWidget(QWidget *parent) : QTabWidget(parent) {
}

void MyQTabWidget::resizeEvent(QResizeEvent *event) {
	QTabWidget::resizeEvent(event);
	emit resized(event->size());
}

ZLQtOptionsDialog::ZLQtOptionsDialog(const std::string &id, const std::string &caption, shared_ptr<ZLRunnable> applyAction) : ZLFullScreenDialog(caption.c_str()), ZLOptionsDialog(id, applyAction) {
	myTabWidget = new MyQTabWidget(this);
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const std::string &name) {
	ZLQtDialogContent *tab = new ZLQtDialogContent(myTabWidget, name);
	myTabWidget->insertTab(tab->widget(), name.c_str());
	myTabs.push_back(tab);
	return *tab;
}

const std::string &ZLQtOptionsDialog::selectedTabName() const {
	return myTabs[myTabWidget->currentPageIndex()]->name();
}

void ZLQtOptionsDialog::selectTab(const std::string &name) {
	for (std::vector<shared_ptr<ZLDialogContent> >::const_iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		if ((*it)->name() == name) {
			myTabWidget->setCurrentPage(it - myTabs.begin());
			break;
		}
	}
}

bool ZLQtOptionsDialog::run() {
	for (std::vector<shared_ptr<ZLDialogContent> >::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		((ZLQtDialogContent&)**it).close();
	}
	bool code = exec();
	((ZLQtDialogManager&)ZLQtDialogManager::instance()).fullScreenWorkaround();
	return code;
}

void ZLQtOptionsDialog::resizeEvent(QResizeEvent *) {
  myTabWidget->resize(size());
}

void ZLQtOptionsDialog::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Key_Return) {
		ZLFullScreenDialog::accept();
	} else {
		ZLFullScreenDialog::keyPressEvent(event);
	}
}
