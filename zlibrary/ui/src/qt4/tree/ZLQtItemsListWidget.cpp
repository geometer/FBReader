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

ZLQtItemsListWidget::ZLQtItemsListWidget(QWidget *parent) : QWidget(parent) {
	QVBoxLayout *listLayout = new QVBoxLayout;
	listLayout->setSizeConstraint(QLayout::SetMinimumSize);
	setLayout(listLayout);
	setFixedWidth(500);
	setFixedHeight(98*6); //TODO make rubber design
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
			connect(item, SIGNAL(clicked(const ZLTreeNode*)), this, SIGNAL(nodeEntered(const ZLTreeNode*)));
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

	QLabel *icon = new QLabel;
	QLabel *title = new QLabel(QString::fromStdString(node->title()));
	QLabel *subtitle = new QLabel(QString::fromStdString(node->subtitle()));

	titlesLayout->addWidget(title);
	titlesLayout->addWidget(subtitle);
	shared_ptr<const ZLImage> image = node->image();
	if (!image.isNull()) {
		ZLNetworkManager::Instance().perform(image->synchronizationData());
		QPixmap pixmap = ZLQtImageUtils::ZLImageToQPixmapWithSize(image, QSize(77,77), Qt::SmoothTransformation);
		//qDebug() << pixmap.isNull();
		icon->setPixmap(pixmap);
	}
	mainLayout->addWidget(icon);
	mainLayout->addLayout(titlesLayout);
	mainLayout->addStretch();
	setLayout(mainLayout);

	setFixedWidth(500);
	setFixedHeight(98);  //TODO make rubber design
}

void ZLQtTreeItem::mousePressEvent(QMouseEvent *event) {
	Q_UNUSED(event);
	emit clicked(myNode);
}



