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

#include <QtGui/QApplication>
#include <QtGui/QLineEdit>
#include <QtGui/QPixmap>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QKeyEvent>

#include <ZLibrary.h>

#include "ZLQtSelectionDialog.h"
#include "ZLQtDialogManager.h"
#include "ZLQtUtil.h"

ZLQListWidget::ZLQListWidget(QWidget *parent) : QListWidget(parent) {
}

void ZLQListWidget::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return) {
		emit returnPressed();
	}
	QListWidget::keyPressEvent(event);
}

ZLQtSelectionDialogItem::ZLQtSelectionDialogItem(QListWidget *listWidget, const ZLTreeNodePtr node) : QListWidgetItem(listWidget), myNode(node) {
	setText(::qtString(node->displayName()));
}

ZLQtSelectionDialog::ZLQtSelectionDialog(const std::string &caption, ZLTreeHandler &handler) : QDialog(qApp->activeWindow()), ZLDesktopSelectionDialog(handler) {
	setWindowTitle(::qtString(caption));

	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	myStateLine = new QLineEdit(this);
	myStateLine->setEnabled(!this->handler().isOpenHandler());
	mainLayout->addWidget(myStateLine);

	myListWidget = new ZLQListWidget(this);
	mainLayout->addWidget(myListWidget);

	QWidget *group = new QWidget(this);
	QGridLayout *buttonLayout = new QGridLayout(group);
	buttonLayout->setColumnStretch(0, 3);
	mainLayout->addWidget(group);

	QPushButton *okButton = new QPushButton(group);
	okButton->setText(::qtButtonName(ZLDialogManager::OK_BUTTON));
	buttonLayout->addWidget(okButton, 0, 1);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(group);
	cancelButton->setText(::qtButtonName(ZLDialogManager::CANCEL_BUTTON));
	buttonLayout->addWidget(cancelButton, 0, 2);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

 	connect(myListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(runNodeSlot()));
 	connect(myListWidget, SIGNAL(returnPressed()), this, SLOT(runNodeSlot()));
 	connect(myStateLine, SIGNAL(returnPressed()), this, SLOT(accept()));

	ZLSelectionDialog::update();
}

ZLQtSelectionDialog::~ZLQtSelectionDialog() {
	for (std::map<std::string,QIcon*>::const_iterator it = myIcons.begin(); it != myIcons.end(); ++it) {
		delete it->second;
	}
}

QIcon &ZLQtSelectionDialog::getIcon(const ZLTreeNodePtr node) {
	const std::string &pixmapName = node->pixmapName();
	std::map<std::string,QIcon*>::const_iterator it = myIcons.find(pixmapName);
	if (it == myIcons.end()) {
		QPixmap pixmap((ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + pixmapName + ".png").c_str());
		QIcon *icon = new QIcon(pixmap);
		myIcons[pixmapName] = icon;
		myListWidget->setIconSize(pixmap.size());
		return *icon;
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
	myStateLine->setText(::qtString(handler().stateDisplayName()));
}

void ZLQtSelectionDialog::updateList() {
	myListWidget->clear();

	const std::vector<ZLTreeNodePtr> &subnodes = handler().subnodes();

	if (subnodes.size() > 0) {
		for (std::vector<ZLTreeNodePtr>::const_iterator it = subnodes.begin(); it != subnodes.end(); ++it) {
		 	QListWidgetItem *item = new ZLQtSelectionDialogItem(myListWidget, *it);
			item->setIcon(getIcon(*it));
		}
	}
}

void ZLQtSelectionDialog::selectItem(int index) {
	if ((index >= 0) && (index < myListWidget->count())) {
		myListWidget->setCurrentRow(index);
	}
}

void ZLQtSelectionDialog::exitDialog() {
	QDialog::accept();
}

void ZLQtSelectionDialog::runNodeSlot() {
	QListWidgetItem *item = myListWidget->currentItem();
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
