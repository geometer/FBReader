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
#include <qlistview.h>
#include <qpixmap.h>

#include <abstract/ZLStringUtil.h>
#include <abstract/ZLFSDir.h>
#include <abstract/ZLZipDir.h>

#include "QOpenFileDialog.h"

static QPixmap *folderPixmap = NULL;
static QPixmap *htmlFilePixmap = NULL;
static QPixmap *zipFilePixmap = NULL;
static QPixmap *unknownFilePixmap = NULL;

QOpenFileDialogItem::QOpenFileDialogItem(QListView *listView, QListViewItem *previous, const QString name, bool dir) : QListViewItem(listView, previous, name), myIsDir(dir) {
	if (folderPixmap == NULL) {
		folderPixmap = new QPixmap("images/FBReader/folder.png");
		htmlFilePixmap = new QPixmap("images/FBReader/html.png");
		zipFilePixmap = new QPixmap("images/FBReader/zipfolder.png");
		unknownFilePixmap = new QPixmap("images/FBReader/unknown.png");
	}
	if (dir) {
		setPixmap(0, *folderPixmap);
	} else if (name.endsWith(".html")) {
		setPixmap(0, *htmlFilePixmap);
	} else if (name.endsWith(".zip")) {
		setPixmap(0, *zipFilePixmap);
	} else {
		setPixmap(0, *unknownFilePixmap);
	}
}

QString QOpenFileDialogItem::name() {
	return text(0);
}

QOpenFileDialog::QOpenFileDialog(const char *caption) : QDialog() {
	setCaption(caption);
	resize(600, 600);

	myMainBox = new QVBox(this);

	myListView = new QListView(myMainBox);
	myListView->addColumn("File Name");

 	connect(myListView, SIGNAL(clicked(QListViewItem*)), this, SLOT(runItem(QListViewItem*)));
 	connect(myListView, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(runItem(QListViewItem*)));

	updateListView();
}

std::string QOpenFileDialog::getOpenFileName(const char *caption) {
	QOpenFileDialog fileDialog(caption);
	if (fileDialog.exec() != Accepted) {
		return std::string();
	}
	QOpenFileDialogItem *item = (QOpenFileDialogItem*)fileDialog.myListView->selectedItem();
	if ((item == NULL) || (item->isDir())) {
		return std::string();
	}
	return fileDialog.myCurrentDir->name() + fileDialog.myCurrentDir->delimiter() + item->name().ascii();
}

void QOpenFileDialog::resizeEvent(QResizeEvent *event) {
	if (event != NULL) {
  	myMainBox->resize(event->size());
	}
}

void QOpenFileDialog::updateListView() {
	myListView->clear();

	QListViewItem *item = NULL;
	if (myCurrentDir->name() != "/") {
	  item = new QOpenFileDialogItem(myListView, NULL, "..", true);
	}

	std::vector<std::string> dirNames;
	myCurrentDir->collectSubDirs(dirNames, true);
	for (std::vector<std::string>::const_iterator it = dirNames.begin(); it != dirNames.end(); it++) {
		if (isDirectoryVisible(*it)) {
	 		item = new QOpenFileDialogItem(myListView, item, it->c_str(), true);
		}
	}

	std::vector<std::string> fileNames;
	myCurrentDir->collectFiles(fileNames, true);
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); it++) {
		if (isFileVisible(*it)) {
	 		item = new QOpenFileDialogItem(myListView, item, it->c_str(), false);
		}
	}
}

void QOpenFileDialog::runItem(QListViewItem *item) {
	if (item == NULL) {
		return;
	}

	QOpenFileDialogItem *dialogItem = (QOpenFileDialogItem*)item;

	if (dialogItem->isDir()) {
		std::string subdir = myCurrentDir->itemName(dialogItem->name().ascii());
		delete myCurrentDir;
		myCurrentDir = new ZLFSDir(subdir);
		updateListView();
	} else if (ZLStringUtil::stringEndsWith(dialogItem->name().ascii(), ".zip")) {
		std::string zip = myCurrentDir->itemName(dialogItem->name().ascii());
		delete myCurrentDir;
		myCurrentDir = new ZLZipDir(zip);
		updateListView();
	} else {
		accept();
	}
}
