/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <stdio.h>
#include <qvbox.h>
#include <qlineedit.h>
#include <qheader.h>
#include <zapplication.h>

#include <ZLibrary.h>

#include "ZLQtSelectionDialog.h"
#include "ZLQtUtil.h"

ZLQtSelectionDialogItem::ZLQtSelectionDialogItem(QListView *listView, QListViewItem *previous, const ZLTreeNodePtr node) : QListViewItem(listView, previous, ::qtString(node->displayName())), myNode(node) {
}

ZLQtSelectionDialogListView::ZLQtSelectionDialogListView(QWidget *parent) : QListView(parent) {
}

void ZLQtSelectionDialogListView::keyPressEvent(QKeyEvent *event) {
	QListViewItem *item = currentItem();
	switch(event->key()) {
		case Key_Enter:
		case Key_Return:
			if (item != 0)
				emit returnPressed(item);
			return;
		case Key_Down:
			item = item->itemBelow();
			if (item == 0) {
				item = firstChild();
			}
			break;
		case Key_Up:
			item = item->itemAbove();
			if (item == 0) {
				item = firstChild();
				while (item->nextSibling() != 0) {
					item = item->nextSibling();
				}
				while (item->itemBelow() != 0) {
					item = item->itemBelow();
				}
			}		
			break;
		default:
			QListView::keyPressEvent(event);
	}

	if (item != 0) {
		setCurrentItem(item);
		ensureItemVisible(item);
	}
}

ZLQtSelectionDialog::ZLQtSelectionDialog(const std::string &caption, ZLTreeHandler &handler) : ZLFullScreenDialog(caption), ZLSelectionDialog(handler) {
	myMainBox = new QVBox(this);
	setContentWidget(myMainBox);
 
	myStateLine = new QLineEdit(myMainBox);
	myStateLine->setEnabled(!this->handler().isOpenHandler());
	myStateLine->setFrame(false);
	QFrame *f = new QFrame(myMainBox);
	f->setFrameStyle(QFrame::HLine | QFrame::Plain);
	myListView = new ZLQtSelectionDialogListView(myMainBox);
	myListView->addColumn("");
	myListView->setFrameStyle(QFrame::NoFrame);
	myListView->setColumnWidth(0, ZApplication::desktop()->width() - ZApplication::style().scrollBarExtent().width());
	myListView->header()->hide();
	myListView->setSorting(-1, true);

	connect(myListView, SIGNAL(clicked(QListViewItem*)), this, SLOT(runNodeSlot()));
	connect(myListView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(accept()));

	ZLSelectionDialog::update();
}

ZLQtSelectionDialog::~ZLQtSelectionDialog() {
	for (std::map<std::string,QPixmap*>::const_iterator it = myPixmaps.begin(); it != myPixmaps.end(); ++it) {
		delete it->second;
	}
}

QPixmap &ZLQtSelectionDialog::getPixmap(const ZLTreeNodePtr node) {
	const std::string &pixmapName = node->pixmapName();
	std::map<std::string,QPixmap*>::const_iterator it = myPixmaps.find(pixmapName);
	if (it == myPixmaps.end()) {
		QPixmap *pixmap = new QPixmap(
									(ZLibrary::ApplicationImageDirectory()
									+ ZLibrary::FileNameDelimiter
									+ pixmapName + ".png").c_str());
		myPixmaps[pixmapName] = pixmap;
		return *pixmap;
	} else {
		return *it->second;
	}
}

void ZLQtSelectionDialog::updateStateLine() {
	myStateLine->setText(::qtString(handler().stateDisplayName()));
}

void ZLQtSelectionDialog::updateList() {
	myListView->clear();

	const std::vector<ZLTreeNodePtr> &subnodes = handler().subnodes();

	if (subnodes.size() > 0) {
		QListViewItem *item = 0;

		for (std::vector<ZLTreeNodePtr>::const_iterator it = subnodes.begin(); it != subnodes.end(); ++it) {
		 	item = new ZLQtSelectionDialogItem(myListView, item, *it);
			item->setPixmap(0, getPixmap(*it));
		}
	}
}

void ZLQtSelectionDialog::selectItem(int index) {
	QListViewItem *item = myListView->firstChild();
	if (item == 0) {
		return;
	}
	for (; index > 0; --index) {
		item = item->nextSibling();
		if (item == 0) {
			return;
		}
	}
	myListView->setSelected(item, true);
	if (item != myListView->firstChild()) {
		myListView->ensureItemVisible(item);
	}
}

void ZLQtSelectionDialog::exitDialog() {
	ZLFullScreenDialog::accept();
}

void ZLQtSelectionDialog::runNodeSlot() {
	QListViewItem *item = myListView->currentItem();
	if (item != 0) {
		runNode(((ZLQtSelectionDialogItem*)item)->node());
	}
}

void ZLQtSelectionDialog::accept() {
	if (handler().isOpenHandler()) {
		runNodeSlot();
	} else {
		runState((const char*)myStateLine->text().utf8());
	}
}

bool ZLQtSelectionDialog::run() {
	return exec() == Accepted;
}
