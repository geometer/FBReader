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

#include <qpe/resource.h>

#include "../../abstract/filesystem/ZLFSDir.h"
#include "../../abstract/filesystem/ZLZipDir.h"
#include "../../abstract/util/ZLStringUtil.h"

#include "QOpenFileDialog.h"

static QPixmap *folderPixmap = 0;
static QPixmap *htmlFilePixmap = 0;
static QPixmap *zipFilePixmap = 0;
static QPixmap *unknownFilePixmap = 0;

QOpenFileDialogItem::QOpenFileDialogItem(QListView *listView, QListViewItem *previous, const QString name, bool dir) : QListViewItem(listView, previous, name), myIsDir(dir) {
	if (folderPixmap == 0) {
		folderPixmap = new QPixmap(Resource::loadPixmap("FBReader/folder"));
		htmlFilePixmap = new QPixmap(Resource::loadPixmap("FBReader/html"));
		zipFilePixmap = new QPixmap(Resource::loadPixmap("FBReader/zipfolder"));
		unknownFilePixmap = new QPixmap(Resource::loadPixmap("FBReader/unknown"));
	}
	if (dir) {
		setPixmap(0, *folderPixmap);
	} else if (ZLStringUtil::stringEndsWith(name.ascii(), ".html")) {
		setPixmap(0, *htmlFilePixmap);
	} else if (ZLStringUtil::stringEndsWith(name.ascii(), ".zip")) {
		setPixmap(0, *zipFilePixmap);
	} else {
		setPixmap(0, *unknownFilePixmap);
	}
}

QString QOpenFileDialogItem::name() {
	return text(0);
}

QOpenFileDialog::QOpenFileDialog(const char *caption) : FullScreenDialog(caption) {
	resize(600, 600);
	
	myMainBox = new QVBox(this);

	myCurrentDirectoryName = new QLineEdit(myMainBox);
	myCurrentDirectoryName->setText(myCurrentDir->name().c_str());
	myCurrentDirectoryName->setEnabled(false);
	myListView = new QListView(myMainBox);
	myListView->addColumn("File Name");

 	connect(myListView, SIGNAL(clicked(QListViewItem*)), this, SLOT(accept()));
 	connect(myListView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(accept()));

	updateListView("..");
}

std::string QOpenFileDialog::getOpenFileName(const char *caption) {
	QOpenFileDialog fileDialog(caption);
	if (fileDialog.exec() != Accepted) {
		return std::string();
	}
	QOpenFileDialogItem *item = (QOpenFileDialogItem*)fileDialog.myListView->selectedItem();
	if ((item == 0) || (item->isDir())) {
		return std::string();
	}
	return fileDialog.myCurrentDir->name() + fileDialog.myCurrentDir->delimiter() + item->name().ascii();
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
	if (myCurrentDir->name() != "/") {
		item = new QOpenFileDialogItem(myListView, 0, "..", true);
		if (selected == "..") {
			selectedItem = item;
		}
	}

	std::vector<std::string> dirNames;
	myCurrentDir->collectSubDirs(dirNames, true);
	for (std::vector<std::string>::const_iterator it = dirNames.begin(); it != dirNames.end(); it++) {
		if (isDirectoryVisible(*it)) {
	 		item = new QOpenFileDialogItem(myListView, item, it->c_str(), true);
			if (*it == selected) {
				selectedItem = item;
			}
		}
	}

	std::vector<std::string> fileNames;
	myCurrentDir->collectFiles(fileNames, true);
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++) {
		if (isFileVisible(*it)) {
	 		item = new QOpenFileDialogItem(myListView, item, it->c_str(), false);
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
		std::string subdir = myCurrentDir->itemName(dialogItem->name().ascii());
		std::string selectedName = (dialogItem->name() == "..") ? myCurrentDir->shortName() : "..";
		delete myCurrentDir;
		myCurrentDir = new ZLFSDir(subdir);
		myCurrentDirectoryName->setText(myCurrentDir->name().c_str());
		updateListView(selectedName);
	} else if (ZLStringUtil::stringEndsWith(dialogItem->name().ascii(), ".zip")) {
		std::string zip = myCurrentDir->itemName(dialogItem->name().ascii());
		delete myCurrentDir;
		myCurrentDir = new ZLZipDir(zip);
		myCurrentDirectoryName->setText(myCurrentDir->name().c_str());
		updateListView("..");
	} else {
		FullScreenDialog::accept();
	}
}
