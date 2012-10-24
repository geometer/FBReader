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

ZLQtTreeDialog::ZLQtTreeDialog(const ZLResource &res, QWidget *parent) : QDialog(parent), ZLTreeDialog(res) {
	setWindowTitle(QString::fromStdString(resource().value())); //TODO maybe user resources by other way
	setMinimumSize(400, 260); //minimum sensible size

	myListWidget = new ZLQtItemsListWidget;
	myPreviewWidget = new ZLQtPreviewWidget;
	myBackButton = new ZLQtIconButton("back_button.png", "back_button_disabled.png");
	myForwardButton = new ZLQtIconButton("forward_button.png", "forward_button_disabled.png");
	mySearchField = new QLineEdit;

	mySearchField->setPlaceholderText("type to search..."); // TODO add to resources;
	//mySearchField->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	mySearchField->setFixedWidth(150);

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
	//panelLayout->addSpacing(10);
	panelLayout->addStretch();
	panelLayout->addWidget(mySearchField);
	//panelLayout->addStretch();
	mainLayout->addLayout(panelLayout);
	mainLayout->addWidget(splitter);
	this->setLayout(mainLayout);

	connect(myListWidget, SIGNAL(nodeClicked(const ZLTreeNode*)), this, SLOT(onNodeClicked(const ZLTreeNode*)));
	connect(myListWidget, SIGNAL(nodeDoubleClicked(const ZLTreeNode*)), this, SLOT(onNodeDoubleClicked(const ZLTreeNode*)));
	connect(myBackButton, SIGNAL(clicked()), this, SLOT(onBackButton()));
	connect(myForwardButton, SIGNAL(clicked()), this, SLOT(onForwardButton()));
}

void ZLQtTreeDialog::run(ZLTreeNode *rootNode) {
	myRootNode = rootNode;
	myBackHistory.clear();
	myForwardHistory.clear();
	onExpandRequest(myRootNode);
	show();
	myListWidget->setFocus();
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
	if (node->children().empty()) {
		node->requestChildren(0); //TODO set listener here
	}
	if (node->children().empty()) {
		return;
	}

	myBackHistory.push(node);
	myForwardHistory.clear();
	myListWidget->fillNodes(myBackHistory.top());
	myListWidget->verticalScrollBar()->setValue(myListWidget->verticalScrollBar()->minimum()); //to the top
	updateAll();
}

void ZLQtTreeDialog::updateAll() {
	updateNavigationButtons();
}

void ZLQtTreeDialog::onCloseRequest() {}

void ZLQtTreeDialog::onNodeBeginInsert(ZLTreeNode */*parent*/, size_t /*index*/) {}

void ZLQtTreeDialog::onNodeEndInsert() {}

void ZLQtTreeDialog::onNodeBeginRemove(ZLTreeNode */*parent*/, size_t /*index*/) {}

void ZLQtTreeDialog::onNodeEndRemove() {}

void ZLQtTreeDialog::onNodeUpdated(ZLTreeNode */*node*/) {}

void ZLQtTreeDialog::updateNavigationButtons() {
	myBackButton->setEnabled(myBackHistory.size() > 1);
	myForwardButton->setEnabled(!myForwardHistory.empty());
	myPreviewWidget->clear();
}

void ZLQtTreeDialog::onNodeClicked(const ZLTreeNode *node) {
	if (const ZLTreePageNode *pageNode = zlobject_cast<const ZLTreePageNode*>(node)) {
		shared_ptr<ZLTreePageInfo> info = pageNode->getPageInfo();
		if (!info.isNull()) {
			myPreviewWidget->fill(*info);
		}
	} else if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
		myPreviewWidget->fillCatalog(titledNode);
	}
}

void ZLQtTreeDialog::onNodeDoubleClicked(const ZLTreeNode *node) {
	if (const ZLTreePageNode *pageNode = zlobject_cast<const ZLTreePageNode*>(node)) {
		(void)pageNode;
		//TODO maybe use different kind of check
		//isExpandable method for i.e.
		return;
	}
	//TODO fix this hack
	ZLTreeNode *clickedNode = const_cast<ZLTreeNode*>(node);
	onExpandRequest(clickedNode);
}

void ZLQtTreeDialog::onBackButton() {
	if (myBackHistory.size() <= 1) {
		return;
	}
	myForwardHistory.push(myBackHistory.pop());
	myListWidget->fillNodes(myBackHistory.top());
	updateAll();
}

void ZLQtTreeDialog::onForwardButton() {
	if (myForwardHistory.empty()) {
		return;
	}
	myBackHistory.push(myForwardHistory.pop());
	myListWidget->fillNodes(myBackHistory.top());
	updateAll();
}

ZLQtIconButton::ZLQtIconButton(QString iconEnabled, QString iconDisabled, QWidget *parent) : QPushButton(parent) {
	QString path = QString::fromStdString(ZLibrary::ApplicationImageDirectory()) +
		QString::fromStdString(ZLibrary::FileNameDelimiter);
	myEnabled = QPixmap(path + iconEnabled);
	myDisabled = QPixmap(path + iconDisabled);
	setIconSize(myEnabled.size());
	setFixedSize(28, 22);
}

void ZLQtIconButton::setEnabled(bool enabled) {
	setIcon(enabled ? myEnabled : myDisabled);
	QPushButton::setEnabled(enabled);
}

