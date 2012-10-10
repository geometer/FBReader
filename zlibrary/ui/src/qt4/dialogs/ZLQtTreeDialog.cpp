/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QScrollBar>
#include <QtCore/QDebug>

#include <ZLTreePageNode.h>

#include "ZLQtTreeDialog.h"

ZLQtTreeDialog::ZLQtTreeDialog(const ZLResource &res, QWidget *parent) : QDialog(parent), ZLTreeDialog(res) {
	setWindowTitle(QString::fromStdString(resource().value())); //TODO maybe user resources by other way

	myScrollArea = new QScrollArea;
	myListWidget = new ZLQtItemsListWidget;
	myPreviewWidget = new ZLQtPreviewWidget;
	myBackButton = new QPushButton("Back"); //TODO add to resources;
	mySearchField = new QLineEdit("type to search..."); // TODO add to resources;

	myScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	myScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	myScrollArea->setWidget(myListWidget);
	myScrollArea->setFrameShape(QFrame::NoFrame);
	myScrollArea->setWidgetResizable(true);
	myScrollArea->setMinimumSize(myListWidget->minimumSize() + QSize(myScrollArea->verticalScrollBar()->width(), 0));
	myScrollArea->setSizePolicy(myListWidget->sizePolicy());

	QVBoxLayout *mainLayout = new QVBoxLayout;
	QHBoxLayout *panelLayout = new QHBoxLayout;

	QSplitter *splitter = new QSplitter;
	splitter->addWidget(myScrollArea);
	splitter->addWidget(myPreviewWidget);
//	splitter->setStretchFactor(0, 2);
	splitter->setStretchFactor(1, 1);

//	mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

	panelLayout->addWidget(myBackButton);
	panelLayout->addWidget(mySearchField);
	//panelLayout->addStretch();
	mainLayout->addLayout(panelLayout);
	mainLayout->addWidget(splitter);
	this->setLayout(mainLayout);

	QPalette palette = myScrollArea->palette();
	palette.setBrush(QPalette::All, QPalette::Window, QColor(242,242,242)); //gray
	myScrollArea->setPalette(palette);

	connect(myListWidget, SIGNAL(nodeClicked(const ZLTreeNode*)), this, SLOT(onNodeClicked(const ZLTreeNode*)));
	connect(myBackButton, SIGNAL(clicked()), this, SLOT(onBackButton()));

}

void ZLQtTreeDialog::run(ZLTreeNode *rootNode) {
	myRootNode = rootNode;
	onExpandRequest(myRootNode);
	show();
}

void ZLQtTreeDialog::onExpandRequest(ZLTreeNode *node) {
	myHistoryStack.push(node);
	myListWidget->fillNodes(myHistoryStack.top());
	myScrollArea->verticalScrollBar()->setValue(myScrollArea->verticalScrollBar()->minimum()); //to the top
	updateBackButton();
}

void ZLQtTreeDialog::onCloseRequest() {}

void ZLQtTreeDialog::onNodeBeginInsert(ZLTreeNode */*parent*/, size_t /*index*/) {}

void ZLQtTreeDialog::onNodeEndInsert() {}

void ZLQtTreeDialog::onNodeBeginRemove(ZLTreeNode */*parent*/, size_t /*index*/) {}

void ZLQtTreeDialog::onNodeEndRemove() {}

void ZLQtTreeDialog::onNodeUpdated(ZLTreeNode */*node*/) {}

void ZLQtTreeDialog::updateBackButton() {
	myBackButton->setEnabled(myHistoryStack.size() > 1);
	myPreviewWidget->clear();
}

void ZLQtTreeDialog::onNodeClicked(const ZLTreeNode *node) {
	if (const ZLTreePageNode *pageNode = zlobject_cast<const ZLTreePageNode*>(node)) {
		shared_ptr<ZLTreePageInfo> info = pageNode->getPageInfo();
		if (!info.isNull()) {
			myPreviewWidget->fill(*info);
		}
	}
}

void ZLQtTreeDialog::onBackButton() {
	if (myHistoryStack.size() <= 1) {
		return;
	}
	myHistoryStack.pop();
	myListWidget->fillNodes(myHistoryStack.top());
	updateBackButton();
}




