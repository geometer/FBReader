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

#include <algorithm>

#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QScrollBar>
#include <QtGui/QResizeEvent>
#include <QtCore/QDebug>

#include <ZLibrary.h>

#include <ZLTreePageNode.h>

#include "../tree/ZLQtItemsListWidget.h"
#include "../tree/ZLQtPreviewWidget.h"

#include "ZLQtTreeDialog.h"

static const int DIALOG_WIDTH_HINT = 840;

ZLQtTreeDialog::ZLQtTreeDialog(const ZLResource &res, QWidget *parent) : QDialog(parent), ZLTreeDialog(res), myLastClickedNode(0) {
	setWindowTitle(QString::fromStdString(resource().value())); //TODO maybe user resources by other way
	setMinimumSize(400, 260); //minimum sensible size

	myListWidget = new ZLQtItemsListWidget;
	myPreviewWidget = new ZLQtPreviewWidget;
	myBackButton = new QPushButton("Back"); //TODO add to resources;
	mySearchField = new QLineEdit("type to search..."); // TODO add to resources;

	QVBoxLayout *mainLayout = new QVBoxLayout;
	QHBoxLayout *panelLayout = new QHBoxLayout;

	QSplitter *splitter = new QSplitter;
	splitter->setChildrenCollapsible(false);
	splitter->addWidget(myListWidget);
	splitter->addWidget(myPreviewWidget);
	splitter->setSizes(QList<int>() << DIALOG_WIDTH_HINT / 2 + myListWidget->verticalScrollBar()->width() << DIALOG_WIDTH_HINT / 2); //50/50 default size

	mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

	panelLayout->addWidget(myBackButton);
	panelLayout->addWidget(mySearchField);
	//panelLayout->addStretch();
	mainLayout->addLayout(panelLayout);
	mainLayout->addWidget(splitter);
	this->setLayout(mainLayout);

	connect(myListWidget, SIGNAL(nodeClicked(ZLQtTreeItem*)), this, SLOT(onNodeClicked(ZLQtTreeItem*)));
	connect(myListWidget, SIGNAL(nodeDoubleClicked(ZLQtTreeItem*)), this, SLOT(onNodeDoubleClicked(ZLQtTreeItem*)));
	connect(myBackButton, SIGNAL(clicked()), this, SLOT(onBackButton()));
}

void ZLQtTreeDialog::run(ZLTreeNode *rootNode) {
	myRootNode = rootNode;
	onChildrenLoaded(myRootNode, false, true); //TODO make generic async loading
	show();
}

void ZLQtTreeDialog::onCloseRequest() {
	hide();
}

QSize ZLQtTreeDialog::sizeHint() const {
	return QSize(DIALOG_WIDTH_HINT + myListWidget->verticalScrollBar()->width(), 0);
}

void ZLQtTreeDialog::resizeEvent(QResizeEvent *event){
	int width = event->size().width();
	int listWidth = width / 3;
	int previewWidth = width / 3;
	myListWidget->setMinimumWidth(listWidth);
	myPreviewWidget->setMinimumWidth(previewWidth);
}

void ZLQtTreeDialog::onExpandRequest(ZLTreeNode *node) {
	myLastClickedNode = node;
	if (node->children().empty()) {
		//expand request is used for RelatedAction, so we don't use waiting icon here
		node->requestChildren(new ChildrenRequestListener(this, node));
	} else {
		onChildrenLoaded(node, false, true);
	}
}

void ZLQtTreeDialog::onChildrenLoaded(const ZLTreeNode *node, bool checkLast, bool successLoaded) {
	if (!successLoaded) {
		return;
	}
	if (node->children().empty()) {
		return;
	}
	if (checkLast) {
		if (node != myLastClickedNode) { //load just last clicked item
			return;
		}
	}
	myLastClickedNode = 0; //for case if item has been requested for several times
	myHistoryStack.push(node);
	myListWidget->fillNodes(myHistoryStack.top());
	myListWidget->verticalScrollBar()->setValue(myListWidget->verticalScrollBar()->minimum()); //to the top
	updateAll();
}

void ZLQtTreeDialog::updateAll() {
	updateBackButton();
	updateWaitingIcons();
}

void ZLQtTreeDialog::updateWaitingIcons() {
	foreach(ZLQtTreeItem *item, myListWidget->getItems()) {
		ZLTreeNode *node = item->getNode();
		if (myDownloadingNodes.contains(node)) {
			item->getWaitingIcon()->start();
		} else {
			item->getWaitingIcon()->finish();
		}
	}
}

void ZLQtTreeDialog::onNodeBeginInsert(ZLTreeNode */*parent*/, size_t /*index*/) {}

void ZLQtTreeDialog::onNodeEndInsert() {}

void ZLQtTreeDialog::onNodeBeginRemove(ZLTreeNode */*parent*/, size_t /*index*/) {}

void ZLQtTreeDialog::onNodeEndRemove() {}

void ZLQtTreeDialog::onNodeUpdated(ZLTreeNode *node) {
	foreach(ZLQtTreeItem *item, myListWidget->getItems()) {
		if (node == item->getNode()) {
			if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(node)) {
				item->fill(titledNode);
				updateAll();
				break;
			}
		}
	}
}

void ZLQtTreeDialog::onDownloadingStarted(ZLTreeNode *node) {
	myDownloadingNodes.insert(node);
	updateWaitingIcons();
}

void ZLQtTreeDialog::onDownloadingStopped(ZLTreeNode *node) {
	myDownloadingNodes.remove(node);
	updateWaitingIcons();
}

void ZLQtTreeDialog::updateBackButton() {
	myBackButton->setEnabled(myHistoryStack.size() > 1);
	myPreviewWidget->clear();
}

void ZLQtTreeDialog::onNodeClicked(ZLQtTreeItem* item) {
	ZLTreeNode* node = item->getNode();
	if (ZLTreePageNode *pageNode = zlobject_cast<ZLTreePageNode*>(node)) {
		shared_ptr<ZLTreePageInfo> info = pageNode->getPageInfo();
		if (!info.isNull()) {
			myPreviewWidget->fill(*info);
		}
	} else if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
		myPreviewWidget->fillCatalog(titledNode);
	}
}

void ZLQtTreeDialog::onNodeDoubleClicked(ZLQtTreeItem* item) {
	const ZLTreeNode* node = item->getNode();
	if (const ZLTreePageNode *pageNode = zlobject_cast<const ZLTreePageNode*>(node)) {
		(void)pageNode;
		//TODO maybe use different kind of check
		//isExpandable method for i.e.
		return;
	}
	onExpandRequest(item->getNode());
}

void ZLQtTreeDialog::onBackButton() {
	if (myHistoryStack.size() <= 1) {
		return;
	}
	myHistoryStack.pop();
	myListWidget->fillNodes(myHistoryStack.top());
	updateAll();
}

ZLQtTreeDialog::ChildrenRequestListener::ChildrenRequestListener(ZLQtTreeDialog *dialog, const ZLTreeNode *node) :
	myTreeDialog(dialog), myNode(node) {
}

void ZLQtTreeDialog::ChildrenRequestListener::finished(const std::string &error) {
	if (!error.empty()) {
		//TODO show error message?
	}
	myTreeDialog->onChildrenLoaded(myNode, true, error.empty());
}
