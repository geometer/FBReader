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

#include <ZLFile.h>
#include <ZLibrary.h>
#include <ZLTreePageNode.h>
#include <ZLFile.h>

#include "../tree/ZLQtItemsListWidget.h"
#include "../tree/ZLQtPreviewWidget.h"

#include "ZLQtTreeDialog.h"

static const int DIALOG_WIDTH_HINT = 840;

ZLQtTreeDialog::ZLQtTreeDialog(const ZLResource &res, QWidget *parent) :
	QDialog(parent), ZLTreeDialog(res), myLastClickedNode(0), myLastClickedSearchNode(0) {
	setWindowTitle(QString::fromStdString(resource().value())); //TODO maybe user resources by other way
	setMinimumSize(400, 260); //minimum sensible size

	myListWidget = new ZLQtItemsListWidget;
	myPreviewWidget = new ZLQtPreviewWidget;
	myBackButton = new ZLQtIconButton("back_button.png", "back_button_disabled.png");
	myForwardButton = new ZLQtIconButton("forward_button.png", "forward_button_disabled.png");
	mySearchField = new ZLQtSearchField;

	myBackButton->setAutoDefault(false);
	myForwardButton->setAutoDefault(false);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	QHBoxLayout *panelLayout = new QHBoxLayout;

	QSplitter *splitter = new QSplitter;
	splitter->setChildrenCollapsible(false);
	splitter->addWidget(myListWidget);
	splitter->addWidget(myPreviewWidget);

	const int scrollbarWidth = 30; //myListWidget->verticalScrollBar()->width() * 2; //commented because with Qt::ScrollBarAsNeeded policy the size is too big
	splitter->setSizes(QList<int>() << DIALOG_WIDTH_HINT / 2 + scrollbarWidth << DIALOG_WIDTH_HINT / 2 - scrollbarWidth); //50/50 default size

	mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

	panelLayout->setSpacing(0);
	panelLayout->addWidget(myBackButton);
	panelLayout->addWidget(myForwardButton);
	panelLayout->addStretch();
	panelLayout->addWidget(mySearchField);

	mainLayout->addLayout(panelLayout);
	mainLayout->addWidget(splitter);
	this->setLayout(mainLayout);

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
	node->requestChildren(new ChildrenRequestListener(this, node, false));
}

void ZLQtTreeDialog::onMoreChildrenRequest(ZLTreeNode *node) {
	//TODO implement the way to not sending new request for more children
	//qDebug() << Q_FUNC_INFO << node << node->children().size();
	//TODO don't ask many times
	node->requestMoreChildren(new ChildrenRequestListener(this, node, true));
}

void ZLQtTreeDialog::onChildrenLoaded(ZLTreeNode *node, bool checkLast, bool successLoaded) {
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
		myListWidget->fillNewNodes(myBackHistory.top());
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

ZLQtTreeDialog::ChildrenRequestListener::ChildrenRequestListener(ZLQtTreeDialog *dialog, ZLTreeNode *node, bool moreMode) :
	myTreeDialog(dialog), myNode(node), myMoreMode(moreMode) {
}

void ZLQtTreeDialog::ChildrenRequestListener::finished(const std::string &error) {
	if (!error.empty()) {
		//TODO show error message?
	}
	if (!myMoreMode) {
		myTreeDialog->onChildrenLoaded(myNode, true, error.empty());
	} else {
		myTreeDialog->onMoreChildrenLoaded(error.empty());
	}

}

ZLQtIconButton::ZLQtIconButton(const std::string &iconEnabled, const std::string &iconDisabled, QWidget *parent) : QPushButton(parent) {
	static std::string imagePrefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
	myEnabled = QPixmap(ZLFile(imagePrefix + iconEnabled).path().c_str());
	myDisabled = QPixmap(ZLFile(imagePrefix + iconDisabled).path().c_str());
	setIconSize(myEnabled.size());
	setFixedSize(28, 22);

	//setAttribute is needed to workaround a bug on MacOS with size for QPushButton-derived subclasses
	//see https://bugreports.qt-project.org/browse/QTBUG-14591
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

void ZLQtIconButton::setEnabled(bool enabled) {
	setIcon(enabled ? myEnabled : myDisabled);
	QPushButton::setEnabled(enabled);
}

