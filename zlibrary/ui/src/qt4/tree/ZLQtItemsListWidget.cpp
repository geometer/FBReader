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

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPixmap>
#include <QtGui/QPalette>

#include <QtCore/QDebug>

#include <ZLNetworkManager.h>
#include <ZLTreeTitledNode.h>

#include "../image/ZLQtImageUtils.h"

#include "ZLQtPreviewWidget.h"

#include "ZLQtItemsListWidget.h"

static const int ITEM_WIDTH = 600;
static const int ITEM_HEIGHT = 98;
static const int ITEM_COUNT = 6;
static const int ITEM_SIZE = 77;

//class ZLQtLabelAction : public QLabel {

//public:
//	ZLQtLabelAction(shared_ptr<ZLTreeAction> action, QWidget *parent=0, Qt::WindowFlags f=0);

//protected:
//	 void mousePressEvent(QMouseEvent *ev);

//private:
//	shared_ptr<ZLTreeAction> myAction;
//};

//ZLQtLabelAction::ZLQtLabelAction(shared_ptr<ZLTreeAction> action,QWidget *parent, Qt::WindowFlags f) :
//	QLabel(parent, f), myAction(action) {
//}

//void ZLQtLabelAction::mousePressEvent(QMouseEvent *) {
//	if (myAction.isNull() || !myAction->makesSense()) {
//		return;
//	}
//	myAction->run();
//}


ZLQtItemsListWidget::ZLQtItemsListWidget(QWidget *parent) : QWidget(parent), myLayout(0) {
//	setFixedWidth(ITEM_WIDTH);
//	setFixedHeight(ITEM_HEIGHT * ITEM_COUNT); //TODO make rubber design

	setMinimumSize(ITEM_WIDTH, ITEM_HEIGHT * ITEM_COUNT);
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void ZLQtItemsListWidget::fillNodes(const ZLTreeNode *expandNode) {
	if (myLayout != 0) {
		delete myLayout;
		qDeleteAll(this->children());
	}
	myLayout = new QVBoxLayout;
	myLayout->setSizeConstraint(QLayout::SetMinimumSize);
	setLayout(myLayout);

	foreach(ZLTreeNode* node, expandNode->children()) {
		if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
			//qDebug() << QString::fromStdString(titledNode->title());
			ZLQtTreeItem *item = new ZLQtTreeItem(titledNode);
			connect(item, SIGNAL(clicked(const ZLTreeNode*)), this, SIGNAL(nodeClicked(const ZLTreeNode*))); //action ExpandAction used instead
			myLayout->addWidget(item);
		}
	}

	myLayout->addStretch();
}

ZLQtTreeItem::ZLQtTreeItem(const ZLTreeTitledNode *node, QWidget *parent) : QWidget(parent), myNode(node) {
	QHBoxLayout *mainLayout = new QHBoxLayout;
	QVBoxLayout *titlesLayout = new QVBoxLayout;
	QHBoxLayout *actionsLayout = new QHBoxLayout;

	QLabel *icon = new QLabel;
	QLabel *title = new QLabel(QString("<b>%1</b>").arg(QString::fromStdString(node->title())));
	QLabel *subtitle = new QLabel(QString::fromStdString(node->subtitle()));
	title->setWordWrap(true);
	subtitle->setWordWrap(true);

	title->setMinimumWidth(ITEM_WIDTH-100);
	subtitle->setMinimumWidth(ITEM_WIDTH-100);

	titlesLayout->addWidget(title);
	titlesLayout->addWidget(subtitle);


	foreach(shared_ptr<ZLTreeAction> action, node->actions()) {
		if (!action->makesSense()) {
			continue;
		}
		QPushButton *actionButton = new ZLQtButtonAction(action);
		QString text = QString::fromStdString(node->actionText(action));
		actionButton->setText(text);
		//actionButton->setCursor(Qt::PointingHandCursor);
		actionsLayout->addWidget(actionButton);
	}

	actionsLayout->addStretch();

	titlesLayout->addLayout(actionsLayout);
	shared_ptr<const ZLImage> image = node->image();
	if (!image.isNull()) {
		ZLNetworkManager::Instance().perform(image->synchronizationData());
		QPixmap pixmap = ZLQtImageUtils::ZLImageToQPixmapWithSize(image, QSize(ITEM_SIZE,ITEM_SIZE), Qt::SmoothTransformation);
		//qDebug() << pixmap.isNull();
		icon->setPixmap(pixmap);
	}
	mainLayout->addWidget(icon);
	mainLayout->addLayout(titlesLayout);
	mainLayout->addStretch();
	setLayout(mainLayout);

	setMinimumSize(ITEM_WIDTH, ITEM_HEIGHT); //TODO make rubber design
}

void ZLQtTreeItem::mousePressEvent(QMouseEvent *) {
	emit clicked(myNode);
}


