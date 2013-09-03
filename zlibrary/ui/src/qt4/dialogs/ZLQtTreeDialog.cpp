/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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
#include <QtGui/QScrollBar>
#include <QtGui/QResizeEvent>
#include <QtGui/QToolBar>

#include <ZLFile.h>
#include <ZLibrary.h>
#include <ZLTreePageNode.h>
#include <ZLFile.h>

#include "../tree/ZLQtItemsListWidget.h"
#include "../tree/ZLQtPreviewWidget.h"
#include "../util/ZLQtUtil.h"
#include "../util/ZLQtToolbarButton.h"
#include "../application/ZLQtApplicationWindow.h"

#include "ZLQtTreeDialog.h"

#include <iostream> //udmv

static const int DIALOG_WIDTH_HINT = 840;

ZLQtTreeDialog::ZLQtTreeDialog(const std::string &windowName, const ZLResource &res, ZLTreeDataProvider *dataProvider) :
	ZLQtMainWindow(ZLQtApplicationWindow::Instance(), windowName),
    ZLTreeDialog(res), myLastClickedNode(0), myLastClickedSearchNode(0), myDataProvider(dataProvider) {

    myDataProvider->setParent(this);

    setWindowTitle(::qtString(resource().value())); //TODO maybe user resources by other way
	setMinimumSize(400, 260); //minimum sensible size

	myListWidget = new ZLQtItemsListWidget;
	myPreviewWidget = new ZLQtPreviewWidget;
	myBackButton = new ZLQtToolbarButton("back_button");
	myForwardButton = new ZLQtToolbarButton("forward_button");
	mySearchField = new ZLQtSearchField;

	QSplitter *splitter = new QSplitter;
	splitter->setChildrenCollapsible(false);
	splitter->addWidget(myListWidget);
	splitter->addWidget(myPreviewWidget);

	const int scrollbarWidth = 30; //myListWidget->verticalScrollBar()->width() * 2; //commented because with Qt::ScrollBarAsNeeded policy the size is too big
	splitter->setSizes(QList<int>() << DIALOG_WIDTH_HINT / 2 + scrollbarWidth << DIALOG_WIDTH_HINT / 2 - scrollbarWidth); //50/50 default size

	myToolbar->addWidget(myBackButton);
	myToolbar->addWidget(myForwardButton);
	QWidget* spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	myToolbar->addWidget(spacer);
	myToolbar->addWidget(mySearchField);

	setCentralWidget(splitter);

	connect(myListWidget, SIGNAL(nodeClicked(ZLQtTreeItem*)), this, SLOT(onNodeClicked(ZLQtTreeItem*)));
	connect(myListWidget, SIGNAL(nodeDoubleClicked(ZLQtTreeItem*)), this, SLOT(onNodeDoubleClicked(ZLQtTreeItem*)));
	connect(myBackButton, SIGNAL(clicked()), this, SLOT(onBackButton()));
	connect(myForwardButton, SIGNAL(clicked()), this, SLOT(onForwardButton()));
	connect(mySearchField, SIGNAL(returnPressed()), this, SLOT(onSearchField()));
	connect(myListWidget, SIGNAL(wantMoreChildren()), this, SLOT(onMoreChildren()));
}

void ZLQtTreeDialog::run(ZLTreeNode *rootNode) {
	myRootNode = rootNode;
	myBackHistory.clear();
	myForwardHistory.clear();
	onExpandRequest(myRootNode); //TODO make generic async loading
	show();
	myListWidget->setFocus();
}

void ZLQtTreeDialog::onCloseRequest() {
    hide();
}

void ZLQtTreeDialog::closeEvent(QCloseEvent *event){
    if (ZLApplication::Instance().closeDialog()) {
        event->accept();
    } else {
        event->ignore();
    }
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
    if(node->children ().size ()>0){
        onChildrenLoaded(node, true, true);
    }else{
        node->requestChildren(myDataProvider->getData(node, false));
    }
}

void ZLQtTreeDialog::onMoreChildrenRequest(ZLTreeNode *node) {
	//TODO implement the way to not sending new request for more children
    //TODO don't ask many times
    std::cout << "[ZLQtTreeDialog] onMoreChildrenRequest" << std::endl;
    node->requestMoreChildren(myDataProvider->getData(node, true));
}

void ZLQtTreeDialog::onChildrenLoaded(ZLTreeNode *node, bool checkLast, bool successLoaded) {
    if (!successLoaded || node->children().empty()) {
		return;
	}
    if (checkLast) {
		if (node != myLastClickedNode) { //load just last clicked item
            return;
		}
    }

    saveShowParameters();
    myLastClickedNode = 0; //for case if item has been requested for several times
    myBackHistory.push(node);
    myForwardHistory.clear();
    if (!myBackHistory.empty()) {
        myListWidget->fillNodes(myBackHistory.top());
    }
	//myListWidget->verticalScrollBar()->setValue(myListWidget->verticalScrollBar()->minimum()); //to the top
    setupShowParameters();
	updateAll();
}

void ZLQtTreeDialog::onMoreChildrenLoaded(bool successLoaded) {
	if (!successLoaded) {
		return;
	}
	if (!myBackHistory.empty()) {
        myListWidget->fillNodes(myBackHistory.top(), true);
	}
	updateAll();
}

void ZLQtTreeDialog::updateAll() {
    updateNavigationButtons();
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

void ZLQtTreeDialog::onNodeUpdated(ZLTreeNode *node) {
	if (!myBackHistory.empty() && myBackHistory.top() == node) {
		onMoreChildrenLoaded(true);
		return;
	}

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

void ZLQtTreeDialog::onSearchStarted(ZLTreeNode *node) {
	//TODO what in case if different searches started or stopped?
	myLastClickedSearchNode = node;
	mySearchField->getWaitingIcon()->start();
}

void ZLQtTreeDialog::onSearchStopped(ZLTreeNode *node) {
	if (node == myLastClickedSearchNode) {
		myLastClickedSearchNode = 0;
		mySearchField->getWaitingIcon()->finish();
	}
}

void ZLQtTreeDialog::onRefresh() {
	myPreviewWidget->refresh();
	//TODO maybe add other refreshes? (list widget, for i.e.)
}

void ZLQtTreeDialog::updateNavigationButtons() {
    myBackButton->setEnabled(myBackHistory.size() > 1);
    myForwardButton->setEnabled(!myForwardHistory.empty());
}

void ZLQtTreeDialog::saveShowParameters() {
	if (myBackHistory.empty()) {
		return;
	}
	ShowParameter parameter;
	parameter.sliderPosition = myListWidget->verticalScrollBar()->value();
	parameter.activeItemNumber = -1;	
	for (int i = 0; i < myListWidget->getItems().size(); ++i) {
		if (myListWidget->getItems().at(i)->isActive()) {
			parameter.activeItemNumber = i;
			break;
		}
	}
	myShowParameters.insert(myBackHistory.top(), parameter);
}

void ZLQtTreeDialog::setupShowParameters() {
	if (myBackHistory.empty()) {
		return;
	}
	if (!myShowParameters.contains(myBackHistory.top())) {
		myPreviewWidget->clear();
		return;
	}
	//TODO implement setting a slider position
	ShowParameter parameter = myShowParameters.value(myBackHistory.top());
	if (parameter.activeItemNumber != -1 && myListWidget->getItems().size() > parameter.activeItemNumber) {
		myListWidget->onNodeClicked(myListWidget->getItems().at(parameter.activeItemNumber));
	} else {
		myPreviewWidget->clear();
	}
}

void ZLQtTreeDialog::onNodeClicked(ZLQtTreeItem* item) {
    std::cout << Q_FUNC_INFO << std::endl;
	ZLTreeNode* node = item->getNode();
	myPreviewWidget->show(node);
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
	if (myBackHistory.size() <= 1) {
		return;
	}
	saveShowParameters();
	myLastClickedNode = 0;
	myForwardHistory.push(myBackHistory.pop());
	myListWidget->fillNodes(myBackHistory.top());
	updateAll();
	setupShowParameters();
}

void ZLQtTreeDialog::onForwardButton() {
	if (myForwardHistory.empty()) {
		return;
	}
	saveShowParameters();
	myLastClickedNode = 0;
	myBackHistory.push(myForwardHistory.pop());
	if (!myBackHistory.empty()) {
		myListWidget->fillNodes(myBackHistory.top());
	}
	updateAll();
	setupShowParameters();
}

void ZLQtTreeDialog::onSearchField() {
	if (mySearcher.isNull()) {
		return;
	}
	if (mySearchField->text().isEmpty()) {
		return;
	}
	mySearcher->simpleSearch(mySearchField->text().toStdString());
}

void ZLQtTreeDialog::onMoreChildren() {
	if (!myBackHistory.empty()) {
		onMoreChildrenRequest(myBackHistory.top());
	}
}
