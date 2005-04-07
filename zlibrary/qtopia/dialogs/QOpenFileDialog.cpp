/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qvbox.h>
#include <qlineedit.h>
#include <qheader.h>

#include <qpe/resource.h>

#include "../../abstract/filesystem/ZLFSDir.h"
#include "../../abstract/filesystem/ZLZipDir.h"
#include "../../abstract/util/ZLStringUtil.h"

#include "QOpenFileDialog.h"

QOpenFileDialogItem::QOpenFileDialogItem(QListView *listView, QListViewItem *previous, const QString name, bool dir) : QListViewItem(listView, previous, name), myIsDir(dir) {
}

QString QOpenFileDialogItem::name() {
	return text(0);
}

QOpenFileDialog::QOpenFileDialog(const char *caption, const ZLFileHandler &handler) : FullScreenDialog(caption), ZLOpenFileDialog(handler) {
	resize(600, 600);
	
	myMainBox = new QVBox(this);

	myCurrentDirectoryName = new QLineEdit(myMainBox);
	myCurrentDirectoryName->setText(QString::fromUtf8(myCurrentDir->name().c_str()));
	myCurrentDirectoryName->setEnabled(false);
	myListView = new QListView(myMainBox);
	myListView->addColumn("");
	myListView->header()->hide();

 	connect(myListView, SIGNAL(clicked(QListViewItem*)), this, SLOT(accept()));
 	connect(myListView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(accept()));

	updateListView("..");
}

QOpenFileDialog::~QOpenFileDialog() {
	for (std::map<std::string,QPixmap*>::const_iterator it = myPixmaps.begin(); it != myPixmaps.end(); it++) {
		delete it->second;
	}
}

QPixmap &QOpenFileDialog::getPixmap(const std::string &fileName, bool dir) {
	std::string pName = handler().pixmapName(fileName, dir);
	std::map<std::string,QPixmap*>::const_iterator it = myPixmaps.find(pName);
	if (it == myPixmaps.end()) {
		QPixmap *pixmap = new QPixmap(Resource::loadPixmap(pName.c_str()));
		myPixmaps[pName] = pixmap;
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

void QOpenFileDialog::updateListView(const std::string &selected) {
	myListView->clear();

	QListViewItem *item = 0;
	QListViewItem *selectedItem = 0;
	if ((myCurrentDir->name() != "/") && (handler().isFileVisible("..", true))) {
		item = new QOpenFileDialogItem(myListView, 0, "..", true);
		item->setPixmap(0, getPixmap("..", true));
		if (selected == "..") {
			selectedItem = item;
		}
	}

	std::vector<std::string> dirNames;
	myCurrentDir->collectSubDirs(dirNames, true);
	for (std::vector<std::string>::const_iterator it = dirNames.begin(); it != dirNames.end(); it++) {
		if (handler().isFileVisible(*it, true)) {
	 		item = new QOpenFileDialogItem(myListView, item, QString::fromUtf8(it->c_str()), true);
			item->setPixmap(0, getPixmap(*it, true));
			if (*it == selected) {
				selectedItem = item;
			}
		}
	}

	std::vector<std::string> fileNames;
	myCurrentDir->collectFiles(fileNames, true);
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++) {
		if (handler().isFileVisible(*it, false)) {
	 		item = new QOpenFileDialogItem(myListView, item, QString::fromUtf8(it->c_str()), false);
			item->setPixmap(0, getPixmap(*it, false));
			if (*it == selected) {
				selectedItem = item;
			}
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

void QOpenFileDialog::accept() {
	QOpenFileDialogItem *dialogItem = (QOpenFileDialogItem*)myListView->currentItem();

	if (dialogItem->isDir()) {
		std::string subdir = myCurrentDir->itemName((const char*)dialogItem->name().utf8());
		std::string selectedName = (dialogItem->name() == "..") ? myCurrentDir->shortName() : "..";
		delete myCurrentDir;
		myCurrentDir = new ZLFSDir(subdir);
		myCurrentDirectoryName->setText(QString::fromUtf8(myCurrentDir->name().c_str()));
		updateListView(selectedName);
	} else if (ZLStringUtil::stringEndsWith((const char*)dialogItem->name().utf8(), ".zip")) {
		std::string zip = myCurrentDir->itemName((const char*)dialogItem->name().utf8());
		delete myCurrentDir;
		myCurrentDir = new ZLZipDir(zip);
		myCurrentDirectoryName->setText(QString::fromUtf8(myCurrentDir->name().c_str()));
		updateListView("..");
	} else {
		handler().accept(myCurrentDir->itemName((const char*)dialogItem->name().utf8()), dialogItem->isDir());
		FullScreenDialog::accept();
	}
}
