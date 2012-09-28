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

#include "ZLQtItemsListWidget.h"

static const int ITEM_WIDTH = 600;
static const int ITEM_HEIGHT = 98;
static const int ITEM_COUNT = 6;
static const int ITEM_SIZE = 77;

class ZLQtLabelAction : public QLabel {

public:
	ZLQtLabelAction(shared_ptr<ZLTreeAction> action, QWidget *parent=0, Qt::WindowFlags f=0);

protected:
	 void mousePressEvent(QMouseEvent *ev);

private:
	shared_ptr<ZLTreeAction> myAction;
};

ZLQtLabelAction::ZLQtLabelAction(shared_ptr<ZLTreeAction> action,QWidget *parent, Qt::WindowFlags f) :
	QLabel(parent, f), myAction(action) {
}

void ZLQtLabelAction::mousePressEvent(QMouseEvent *) {
	if (myAction.isNull() || !myAction->makesSense()) {
		return;
	}
	myAction->run();
}


ZLQtItemsListWidget::ZLQtItemsListWidget(QWidget *parent) : QWidget(parent) {
	QVBoxLayout *listLayout = new QVBoxLayout;
	listLayout->setSizeConstraint(QLayout::SetMinimumSize);
	setLayout(listLayout);
	setFixedWidth(ITEM_WIDTH);
	setFixedHeight(ITEM_HEIGHT*ITEM_COUNT); //TODO make rubber design
}

void ZLQtItemsListWidget::fillNodes(const ZLTreeNode *expandNode) {
	foreach(ZLQtTreeItem *item, myItems) {
		layout()->removeWidget(item);
		delete item;
	}
	myItems.clear();

	foreach(ZLTreeNode* node, expandNode->children()) {
		if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
			//qDebug() << QString::fromStdString(titledNode->title());
			ZLQtTreeItem *item = new ZLQtTreeItem(titledNode);
			//connect(item, SIGNAL(clicked(const ZLTreeNode*)), this, SIGNAL(nodeEntered(const ZLTreeNode*))); //action ExpandAction used instead
			myItems.push_back(item);
		}
	}

	foreach(ZLQtTreeItem *item, myItems) {
		layout()->addWidget(item);
	}
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
		QLabel *actionLabel = new ZLQtLabelAction(action);
		QString text = QString::fromStdString(node->actionText(action));
		actionLabel->setText(QString("<small><u>%1</u></small>").arg(text));

		QPalette palette = actionLabel->palette();
		palette.setColor(QPalette::WindowText, QColor(33, 96, 180)); //blue color
		actionLabel->setPalette(palette);
		actionLabel->setCursor(Qt::PointingHandCursor);
		actionsLayout->addWidget(actionLabel);
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

	setFixedWidth(ITEM_WIDTH);
	setFixedHeight(ITEM_HEIGHT);  //TODO make rubber design
}

void ZLQtTreeItem::mousePressEvent(QMouseEvent *event) {
	Q_UNUSED(event);
	emit clicked(myNode);
}


