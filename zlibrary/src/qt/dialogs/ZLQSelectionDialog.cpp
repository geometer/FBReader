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

#include <ZLApplication.h>

#include "ZLQSelectionDialog.h"
#include "QDialogManager.h"

ZLQSelectionDialogItem::ZLQSelectionDialogItem(QListView *listView, QListViewItem *previous, const ZLTreeNodePtr node) : QListViewItem(listView, previous, QString::fromUtf8(node->displayName().c_str())), myNode(node) {
}

ZLQSelectionDialog::ZLQSelectionDialog(const char *caption, ZLTreeHandler &handler) : QDialog(), ZLDesktopSelectionDialog(handler) {
	setCaption(caption);

	myMainBox = new QVBox(this);

	myStateLine = new QLineEdit(myMainBox);
	myStateLine->setEnabled(this->handler().isWriteable());
	myListView = new QListView(myMainBox);
	myListView->addColumn("");
	myListView->header()->hide();
	myListView->setSorting(-1, true);

 	connect(myListView, SIGNAL(clicked(QListViewItem*)), this, SLOT(accept()));
 	connect(myListView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(accept()));

	update("");
}

ZLQSelectionDialog::~ZLQSelectionDialog() {
	for (std::map<std::string,QPixmap*>::const_iterator it = myPixmaps.begin(); it != myPixmaps.end(); ++it) {
		delete it->second;
	}
}

QPixmap &ZLQSelectionDialog::getPixmap(const ZLTreeNodePtr node) {
	const std::string &pixmapName = node->pixmapName();
	std::map<std::string,QPixmap*>::const_iterator it = myPixmaps.find(pixmapName);
	if (it == myPixmaps.end()) {
		QPixmap *pixmap = new QPixmap((ZLApplication::ImageDirectory() + ZLApplication::PathDelimiter + pixmapName + ".png").c_str());
		myPixmaps[pixmapName] = pixmap;
		return *pixmap;
	} else {
		return *it->second;
	}
}

void ZLQSelectionDialog::keyPressEvent(QKeyEvent *event) {
	if ((event != 0) && (event->key() == Key_Escape)) {
		reject();
	}
}

void ZLQSelectionDialog::resizeEvent(QResizeEvent *event) {
	if (event != 0) {
		myMainBox->resize(event->size());
	}
}

void ZLQSelectionDialog::update(const std::string &selectedNodeId) {
	myStateLine->setText(QString::fromUtf8(handler().stateDisplayName().c_str()));

	myListView->clear();

	const std::vector<ZLTreeNodePtr> &subnodes = handler().subnodes();

	if (subnodes.size() > 0) {
		QListViewItem *item = 0;
		QListViewItem *selectedItem = 0;

		for (std::vector<ZLTreeNodePtr>::const_iterator it = subnodes.begin(); it != subnodes.end(); ++it) {
		 	item = new ZLQSelectionDialogItem(myListView, item, *it);
			item->setPixmap(0, getPixmap(*it));
			if ((*it)->id() == selectedNodeId) {
				selectedItem = item;
			}
		}

		if ((selectedItem == 0) && !handler().isWriteable()) {
			selectedItem = myListView->firstChild();
		}
		if (selectedItem != 0) {
			myListView->setSelected(selectedItem, true);
			if (selectedItem != myListView->firstChild()) {
				myListView->ensureItemVisible(selectedItem);
			}
		}
	}
}

void ZLQSelectionDialog::exitDialog() {
	QDialog::accept();
}

void ZLQSelectionDialog::accept() {
	runNode(((ZLQSelectionDialogItem*)myListView->currentItem())->node());
}
