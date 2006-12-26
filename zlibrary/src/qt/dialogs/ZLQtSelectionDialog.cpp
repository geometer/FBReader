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
#include <qlayout.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>

#include <ZLApplication.h>

#include "ZLQtSelectionDialog.h"
#include "QDialogManager.h"

ZLQtSelectionDialogItem::ZLQtSelectionDialogItem(QListView *listView, QListViewItem *previous, const ZLTreeNodePtr node) : QListViewItem(listView, previous, QString::fromUtf8(node->displayName().c_str())), myNode(node) {
}

ZLQtSelectionDialog::ZLQtSelectionDialog(const char *caption, ZLTreeHandler &handler) : QDialog(), ZLDesktopSelectionDialog(handler) {
	setCaption(caption);

	myMainBox = new QVBox(this);

	myStateLine = new QLineEdit(myMainBox);
	myStateLine->setEnabled(!this->handler().isOpenHandler());
	myListView = new QListView(myMainBox);
	myListView->addColumn("");
	myListView->header()->hide();
	myListView->setSorting(-1, true);

	QButtonGroup *group = new QButtonGroup(myMainBox);
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

 	connect(myListView, SIGNAL(clicked(QListViewItem*)), this, SLOT(runNodeSlot()));
 	connect(myListView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(accept()));
 	connect(myStateLine, SIGNAL(returnPressed()), this, SLOT(accept()));

	update("");
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
		QPixmap *pixmap = new QPixmap((ZLApplication::ImageDirectory() + ZLApplication::PathDelimiter + pixmapName + ".png").c_str());
		myPixmaps[pixmapName] = pixmap;
		return *pixmap;
	} else {
		return *it->second;
	}
}

void ZLQtSelectionDialog::keyPressEvent(QKeyEvent *event) {
	if ((event != 0) && (event->key() == Key_Escape)) {
		reject();
	}
}

void ZLQtSelectionDialog::resizeEvent(QResizeEvent *event) {
	if (event != 0) {
		myMainBox->resize(event->size());
	}
}

void ZLQtSelectionDialog::update(const std::string &selectedNodeId) {
	myStateLine->setText(QString::fromUtf8(handler().stateDisplayName().c_str()));

	myListView->clear();

	const std::vector<ZLTreeNodePtr> &subnodes = handler().subnodes();

	if (subnodes.size() > 0) {
		QListViewItem *item = 0;
		QListViewItem *selectedItem = 0;

		for (std::vector<ZLTreeNodePtr>::const_iterator it = subnodes.begin(); it != subnodes.end(); ++it) {
		 	item = new ZLQtSelectionDialogItem(myListView, item, *it);
			item->setPixmap(0, getPixmap(*it));
			if ((*it)->id() == selectedNodeId) {
				selectedItem = item;
			}
		}

		if ((selectedItem == 0) && handler().isOpenHandler()) {
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

void ZLQtSelectionDialog::exitDialog() {
	QDialog::accept();
}

void ZLQtSelectionDialog::runNodeSlot() {
	runNode(((ZLQtSelectionDialogItem*)myListView->currentItem())->node());
}

void ZLQtSelectionDialog::accept() {
	if (handler().isOpenHandler()) {
		runNode(((ZLQtSelectionDialogItem*)myListView->currentItem())->node());
	} else {
		runState((const char*)myStateLine->text().utf8());
	}
}

bool ZLQtSelectionDialog::run() {
	return QDialog::exec() == Accepted;
}
