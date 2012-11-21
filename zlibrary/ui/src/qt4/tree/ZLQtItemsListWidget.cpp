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
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QScrollBar>
#include <QtCore/QTimer>

#include <QtCore/QDebug>

#include <ZLibrary.h>
#include <ZLNetworkManager.h>
#include <ZLTreeTitledNode.h>
#include <ZLFile.h>

#include "../dialogs/ZLQtDialogManager.h"

#include "../image/ZLQtImageUtils.h"

#include "ZLQtPreviewWidget.h"

#include "ZLQtItemsListWidget.h"

static const int ITEM_HEIGHT = 98;
static const int ITEM_COUNT = 5;
static const int ITEM_SIZE = 77;
static const int WAITING_SIZE = 40;

ZLQtItemsListWidget::ZLQtItemsListWidget(QWidget *parent) : QScrollArea(parent), myLayout(0) {

	myContainerWidget = new QWidget;
	myContainerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

	verticalScrollBar()->setTracking(false);
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(onSliderMoved(int)));

	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setWidget(myContainerWidget);
	setFrameShape(QFrame::NoFrame);
	setWidgetResizable(true);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

}

void ZLQtItemsListWidget::clear() {
	if (myLayout != 0) {
		delete myLayout;
		qDeleteAll(myContainerWidget->children());
	}
	myLayout = new QVBoxLayout;
	myLayout->setContentsMargins(0,0,0,0);
	myLayout->setSpacing(0);
	myLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	myContainerWidget->setLayout(myLayout);


	myItems.clear();

}

void ZLQtItemsListWidget::addNode(ZLTreeTitledNode *titledNode) {
	//qDebug() << QString::fromStdString(titledNode->title());
	ZLQtTreeItem *item = new ZLQtTreeItem;
	item->fill(titledNode);
	connect(item, SIGNAL(clicked(ZLQtTreeItem*)), this, SLOT(onNodeClicked(ZLQtTreeItem*))); //action ExpandAction used instead
	connect(item, SIGNAL(doubleClicked(ZLQtTreeItem*)), this, SIGNAL(nodeDoubleClicked(ZLQtTreeItem*)));
	myLayout->addWidget(item);
	myItems.push_back(item);
}

void ZLQtItemsListWidget::fillNodes(const ZLTreeNode *expandNode) {
	clear();

	foreach(ZLTreeNode* node, expandNode->children()) {
		if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(node)) {
			addNode(titledNode);
		}
	}
}

void ZLQtItemsListWidget::fillNewNodes(const ZLTreeNode *rootNode) {
	if (myLayout == 0) {
		return;
	}

	size_t oldSize = (size_t)myItems.size();

	//qDebug() << Q_FUNC_INFO << oldSize << rootNode->children().size();

	for (size_t i = oldSize; i < rootNode->children().size(); ++i) {
		if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(rootNode->children().at(i))) {
			addNode(titledNode);
		}
	}
}

QSize ZLQtItemsListWidget::sizeHint() const {
	return QSize(0, ITEM_HEIGHT * ITEM_COUNT);
}

void ZLQtItemsListWidget::setMinimumWidth(int w) {
	myContainerWidget->setMinimumWidth(w - verticalScrollBar()->width());
	QScrollArea::setMinimumWidth(w);
}

QList<ZLQtTreeItem *> ZLQtItemsListWidget::getItems() const {
	return myItems;
}

void ZLQtItemsListWidget::onNodeClicked(ZLQtTreeItem* itemClicked) {
	foreach(ZLQtTreeItem *item, myItems) {
		item->setActive(item == itemClicked);
	}
	emit nodeClicked(itemClicked);
}

void ZLQtItemsListWidget::onSliderMoved(int value) {
	int maximum = verticalScrollBar()->maximum();
	if ((double)value > (double)maximum * 0.8) {
		emit wantMoreChildren();
	}
}

ZLQtTreeItem::ZLQtTreeItem(QWidget *parent) : QFrame(parent), myNode(0), myImageRequested(false) {
	setAutoFillBackground(true);
	setActive(false);

	QHBoxLayout *mainLayout = new QHBoxLayout;
	QVBoxLayout *titlesLayout = new QVBoxLayout;

	myIcon = new QLabel;
	myTitle = new QLabel;
	mySubtitle = new QLabel;

	myWaitingIcon = new QtWaitingSpinner(12, 6, 4, 10);
	myWaitingIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	myTitle->setWordWrap(true);
	mySubtitle->setWordWrap(true);

//	QSizePolicy policy(QSizePolicy::Maximum, QSizePolicy::Maximum);
//	//policy.setHorizontalStretch(1);
//	myTitle->setSizePolicy(policy);
//	mySubtitle->setSizePolicy(policy);

	titlesLayout->addWidget(myTitle);
	titlesLayout->addWidget(mySubtitle);
	titlesLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

	mainLayout->addWidget(myIcon);
	mainLayout->addLayout(titlesLayout);
	mainLayout->addStretch();
	mainLayout->addWidget(myWaitingIcon, 1);
	setLayout(mainLayout);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setFixedHeight(ITEM_HEIGHT);

	clear();
}

void ZLQtTreeItem::resizeEvent(QResizeEvent *ev) {
	int maxWidth = ev->size().width() - WAITING_SIZE - ITEM_SIZE - 40;
	if (maxWidth <= 0) {
		return;
	}
	myTitle->setFixedWidth(maxWidth);
	mySubtitle->setFixedWidth(maxWidth);
}

void ZLQtTreeItem::fill(ZLTreeTitledNode *node) {
	clear();
	myNode = node;
	myTitle->setText(QString("<b>%1</b>").arg(QString::fromStdString(node->title())));
	mySubtitle->setText(QString::fromStdString(node->subtitle()));
	fillImage();
}

void ZLQtTreeItem::fillImage() {
	//separated method for deferred loding of cover (good for slow connections)
	//TODO as it starts loading on paintEvent, it works bad while scrolling
	if (!myImageRequested) {
		return;
	}
	shared_ptr<const ZLImage> image = myNode->image();
	if (!image.isNull()) {
		QPixmap pixmap = ZLQtImageUtils::ZLImageToQPixmapWithSize(image, QSize(ITEM_SIZE,ITEM_SIZE), false, Qt::SmoothTransformation);
		if (!pixmap.isNull()) {
			myIcon->setPixmap(pixmap);
		}
	}
}

void ZLQtTreeItem::clear() {
	myNode = 0;
	myImageRequested = false;
	myTitle->clear();
	mySubtitle->clear();

	QPixmap pixmap(ITEM_SIZE, ITEM_SIZE);
	pixmap.fill(Qt::transparent);
	myIcon->setPixmap(pixmap);

	myWaitingIcon->finish();

}
void ZLQtTreeItem::setActive(bool active) {
	myIsActive = active;

	//TODO due to different themes on OS, take a color from existed palette
	QColor mainColor = myIsActive ? QColor::fromHsv(0, 0, 0.75 * 255) : QColor::fromHsv(0, 0, 0.95 * 255);

	setFrameStyle(QFrame::Panel | QFrame::Raised);
	setLineWidth(2);

	QPalette p = palette();
	p.setColor(QPalette::Window, mainColor);
	setPalette(p);

	update();
}

bool ZLQtTreeItem::isActive() const {
	return myIsActive;
}

ZLTreeTitledNode *ZLQtTreeItem::getNode() const {
	return myNode;
}

QtWaitingSpinner *ZLQtTreeItem::getWaitingIcon() const {
	return myWaitingIcon;
}

void ZLQtTreeItem::mousePressEvent(QMouseEvent *) {
	emit clicked(this);
}

void ZLQtTreeItem::mouseDoubleClickEvent(QMouseEvent *) {
	emit doubleClicked(this);
}

void ZLQtTreeItem::paintEvent(QPaintEvent *event) {
//	QString title = myNode ? QString::fromStdString(myNode->title()) : "not filled";
//	qDebug() << Q_FUNC_INFO<< title;
	if (myNode && !myImageRequested) {
		//deferred loading of cover, asking on paint event
		myNode->image();
		myImageRequested = true;
		fillImage();
	}
	QFrame::paintEvent(event);
	return;
//	QColor mainColor = isActive ? QColor::fromHsv(0, 0, 0.75 * 255) : QColor::fromHsv(0, 0, 0.95 * 255);
//	int h = mainColor.hue();
//	int s = mainColor.saturation();
//	int v = mainColor.value();
//	QColor shadowColor1 = QColor::fromHsv(h,s,v - 23); //these numbers are getted from experiments with Photo editor
//	QColor shadowColor2 = QColor::fromHsv(h,s,v - 43);
//	QColor shadowColor3 = QColor::fromHsv(h,s,v - 71);
//	QColor shadowColor4 = QColor::fromHsv(h,s,v - 117);
//	QColor shadowColor5 = QColor::fromHsv(h,s,v - 155);

//	QColor shadowUpColor = QColor::fromHsv(h,s,v - 114);



//	//QColor shadow(196,193,189); //TODO not hardcode the color automatically

//	QPainter painter(this);
//	QRect rect = this->rect();

//	//painter.setBrush(mainColor);
//	painter.fillRect(rect, mainColor);

//	painter.setPen(shadowColor5);
//	painter.drawLine(rect.left() + 2, rect.bottom(), rect.right(), rect.bottom());

//	painter.setPen(shadowColor4);
//	//painter.drawLine(rect.left() + 2, rect.top() + 1, rect.right() - 2, rect.top() + 1);

//	painter.setPen(shadowColor3);
//	painter.drawLine(rect.left() + 4, rect.bottom() - 1, rect.right(), rect.bottom() - 1);

//	//painter.drawLine(rect.left() + 2, rect.top(), rect.right() - 2, rect.top());

//	painter.drawLine(rect.right(), rect.top() + 2, rect.right(), rect.bottom() - 1);
//	painter.drawLine(rect.left() + 1, rect.top(), rect.left() + 1, rect.bottom() - 1);

//	painter.setPen(shadowColor2);
//	painter.drawLine(rect.left(), rect.top() + 1, rect.left(), rect.bottom() - 2);
//	painter.drawLine(rect.right() - 1, rect.top() + 2, rect.right() - 1, rect.bottom() - 2);

//	painter.setPen(shadowColor1);
//	painter.drawLine(rect.left() + 5, rect.bottom() - 2, rect.right() - 2, rect.bottom() - 2);
//	painter.drawLine(rect.right() - 2, rect.top() + 2, rect.right() - 2, rect.bottom() - 2);

//	painter.setPen(shadowUpColor);
	//	painter.drawLine(rect.left() + 2, rect.top(), rect.right() - 2, rect.top());

}
