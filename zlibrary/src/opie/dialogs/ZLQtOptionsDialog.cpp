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

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogContent.h"

MyQTabWidget::MyQTabWidget(QWidget *parent) : QTabWidget(parent) {
}

void MyQTabWidget::resizeEvent(QResizeEvent *event) {
	QTabWidget::resizeEvent(event);
	emit resized(event->size());
}

ZLQtOptionsDialog::ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction) : ZLFullScreenDialog(std::string()), ZLOptionsDialog(resource, applyAction) {
	setCaption(QString::fromUtf8(ZLOptionsDialog::caption().c_str()));
	myTabWidget = new MyQTabWidget(this);
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const ZLResourceKey &key) {
	ZLQtDialogContent *tab = new ZLQtDialogContent(myTabWidget);
	myTabWidget->insertTab(tab->widget(), QString::fromUtf8(tabName(key).c_str()));
	myTabs.push_back(tab);
	myTabNames.push_back(key.Name);
	return *tab;
}

const std::string &ZLQtOptionsDialog::selectedTabKey() const {
	return myTabNames[myTabWidget->currentPageIndex()];
}

void ZLQtOptionsDialog::selectTab(const ZLResourceKey &key) {
	std::vector<std::string>::const_iterator it = std::find(myTabNames.begin(), myTabNames.end(), key.Name);
	if (it != myTabNames.end()) {
		myTabWidget->setCurrentPage(it - myTabNames.begin());
	}
}

bool ZLQtOptionsDialog::runInternal() {
	for (std::vector<shared_ptr<ZLDialogContent> >::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		((ZLQtDialogContent&)**it).close();
	}
	return exec();
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
