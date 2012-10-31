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
	splitter->setSizes(QList<int>() << DIALOG_WIDTH_HINT / 2 + myListWidget->verticalScrollBar()->width() << DIALOG_WIDTH_HINT / 2); //50/50 default size

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
	myBackHistory.push(node);
	myForwardHistory.clear();
	myListWidget->fillNodes(myBackHistory.top());
	myListWidget->verticalScrollBar()->setValue(myListWidget->verticalScrollBar()->minimum()); //to the top
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
	myPreviewWidget->clear();
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
	myLastClickedNode = 0;
	myForwardHistory.push(myBackHistory.pop());
	myListWidget->fillNodes(myBackHistory.top());
	updateAll();
}

void ZLQtTreeDialog::onForwardButton() {
	if (myForwardHistory.empty()) {
		return;
	}
	myLastClickedNode = 0;
	myBackHistory.push(myForwardHistory.pop());
	myListWidget->fillNodes(myBackHistory.top());
	updateAll();
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

ZLQtTreeDialog::ChildrenRequestListener::ChildrenRequestListener(ZLQtTreeDialog *dialog, const ZLTreeNode *node) :
	myTreeDialog(dialog), myNode(node) {
}

void ZLQtTreeDialog::ChildrenRequestListener::finished(const std::string &error) {
	if (!error.empty()) {
		//TODO show error message?
	}
	myTreeDialog->onChildrenLoaded(myNode, true, error.empty());
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

