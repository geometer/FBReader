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

#include <QtGui/QApplication>
#include <QtGui/QLineEdit>
#include <QtGui/QPixmap>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QKeyEvent>
#include <Qt3Support/Q3Header>

#include <ZLApplication.h>

#include "ZLQtSelectionDialog.h"
#include "ZLQtDialogManager.h"

ZLQtSelectionDialogItem::ZLQtSelectionDialogItem(Q3ListView *listView, Q3ListViewItem *previous, const ZLTreeNodePtr node) : Q3ListViewItem(listView, previous, QString::fromUtf8(node->displayName().c_str())), myNode(node) {
}

ZLQtSelectionDialog::ZLQtSelectionDialog(const char *caption, ZLTreeHandler &handler) : QDialog(qApp->activeWindow()), ZLDesktopSelectionDialog(handler) {
	setWindowTitle(caption);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	myStateLine = new QLineEdit(this);
	myStateLine->setEnabled(!this->handler().isOpenHandler());
	mainLayout->addWidget(myStateLine);

	myListView = new Q3ListView(this);
	myListView->addColumn("");
	myListView->header()->hide();
	myListView->setSorting(-1, true);
	mainLayout->addWidget(myListView);

	QWidget *group = new QWidget(this);
	QGridLayout *buttonLayout = new QGridLayout(group);
	buttonLayout->setColumnStretch(0, 3);
	buttonLayout->setColumnStretch(1, 0);
	buttonLayout->setColumnStretch(2, 1);
	buttonLayout->setColumnStretch(3, 0);
	buttonLayout->setColumnStretch(4, 3);
	mainLayout->addWidget(group);

	QPushButton *okButton = new QPushButton(group);
	okButton->setText("OK");
	buttonLayout->addWidget(okButton, 0, 1);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(group);
	cancelButton->setText("Cancel");
	buttonLayout->addWidget(cancelButton, 0, 3);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

 	connect(myListView, SIGNAL(clicked(Q3ListViewItem*)), this, SLOT(runNodeSlot()));
 	connect(myListView, SIGNAL(returnPressed(Q3ListViewItem*)), this, SLOT(accept()));
 	connect(myStateLine, SIGNAL(returnPressed()), this, SLOT(accept()));

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
		QPixmap *pixmap = new QPixmap((ZLApplication::ApplicationImageDirectory() + ZLApplication::FileNameDelimiter + pixmapName + ".png").c_str());
		myPixmaps[pixmapName] = pixmap;
		return *pixmap;
	} else {
		return *it->second;
	}
}

void ZLQtSelectionDialog::keyPressEvent(QKeyEvent *event) {
	if ((event != 0) && (event->key() == Qt::Key_Escape)) {
		reject();
	}
}

void ZLQtSelectionDialog::updateStateLine() {
	myStateLine->setText(QString::fromUtf8(handler().stateDisplayName().c_str()));
}

void ZLQtSelectionDialog::updateList() {
	myListView->clear();

	const std::vector<ZLTreeNodePtr> &subnodes = handler().subnodes();

	if (subnodes.size() > 0) {
		Q3ListViewItem *item = 0;

		for (std::vector<ZLTreeNodePtr>::const_iterator it = subnodes.begin(); it != subnodes.end(); ++it) {
		 	item = new ZLQtSelectionDialogItem(myListView, item, *it);
			item->setPixmap(0, getPixmap(*it));
		}
	}
}

void ZLQtSelectionDialog::selectItem(int index) {
	Q3ListViewItem *item = myListView->firstChild();
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
	QDialog::accept();
}

void ZLQtSelectionDialog::runNodeSlot() {
	Q3ListViewItem *item = myListView->currentItem();
	if (item != 0) {
		runNode(((ZLQtSelectionDialogItem*)item)->node());
	}
}

void ZLQtSelectionDialog::accept() {
	if (handler().isOpenHandler()) {
		runNodeSlot();
	} else {
		runState((const char*)myStateLine->text().toUtf8());
	}
}

bool ZLQtSelectionDialog::run() {
	return QDialog::exec() == Accepted;
}
