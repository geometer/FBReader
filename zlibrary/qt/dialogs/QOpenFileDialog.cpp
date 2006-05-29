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

#include <qvbox.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qpixmap.h>

#include "QOpenFileDialog.h"
#include "QDialogManager.h"

QOpenFileDialogItem::QOpenFileDialogItem(QListView *listView, QListViewItem *previous, const ZLTreeNodePtr node) : QListViewItem(listView, previous, QString::fromLocal8Bit(node->name().c_str())), myNode(node) {
}

QOpenFileDialog::QOpenFileDialog(const char *caption, const ZLTreeHandler &handler) : QDialog(), ZLDesktopOpenFileDialog(handler) {
	setCaption(caption);

	myMainBox = new QVBox(this);

	myStateLine = new QLineEdit(myMainBox);
	myStateLine->setEnabled(false);
	myListView = new QListView(myMainBox);
	myListView->addColumn("");
	myListView->header()->hide();

 	connect(myListView, SIGNAL(clicked(QListViewItem*)), this, SLOT(accept()));
 	connect(myListView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(accept()));

	update("");
}

QOpenFileDialog::~QOpenFileDialog() {
	for (std::map<std::string,QPixmap*>::const_iterator it = myPixmaps.begin(); it != myPixmaps.end(); it++) {
		delete it->second;
	}
}

QPixmap &QOpenFileDialog::getPixmap(const ZLTreeNodePtr node) {
	const std::string &pixmapName = node->pixmapName();
	std::map<std::string,QPixmap*>::const_iterator it = myPixmaps.find(pixmapName);
	if (it == myPixmaps.end()) {
		QPixmap *pixmap = new QPixmap((((QDialogManager&)QDialogManager::instance()).getPixmapPath() + "/" + pixmapName + ".png").c_str());
		myPixmaps[pixmapName] = pixmap;
		return *pixmap;
	} else {
		return *it->second;
	}
}

void QOpenFileDialog::keyPressEvent(QKeyEvent *event) {
	if ((event != 0) && (event->key() == Key_Escape)) {
		reject();
	}
}

void QOpenFileDialog::resizeEvent(QResizeEvent *event) {
	if (event != 0) {
		myMainBox->resize(event->size());
	}
}

void QOpenFileDialog::update(const std::string &selectedNodeName) {
	myStateLine->setText(QString::fromLocal8Bit(state()->name().c_str()));

	myListView->clear();

	QListViewItem *item = 0;
	QListViewItem *selectedItem = 0;

	const std::vector<ZLTreeNodePtr> &subnodes = state()->subnodes();
	for (std::vector<ZLTreeNodePtr>::const_iterator it = subnodes.begin(); it != subnodes.end(); it++) {
	 	item = new QOpenFileDialogItem(myListView, item, *it);
		item->setPixmap(0, getPixmap(*it));
		if ((*it)->name() == selectedNodeName) {
			selectedItem = item;
		}
	}

	if (selectedItem == 0) {
		selectedItem = myListView->firstChild();
	}
	myListView->setSelected(selectedItem, true);
	if (selectedItem != myListView->firstChild()) {
		myListView->ensureItemVisible(selectedItem);
	}
}

void QOpenFileDialog::exitDialog() {
	QDialog::accept();
}

void QOpenFileDialog::accept() {
	runNode(((QOpenFileDialogItem*)myListView->currentItem())->node());
}
