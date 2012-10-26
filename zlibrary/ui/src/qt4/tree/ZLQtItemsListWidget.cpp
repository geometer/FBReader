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

ZLQtItemsListWidget::ZLQtItemsListWidget(QWidget *parent) : QScrollArea(parent), myLayout(0) {

	myContainerWidget = new QWidget;
	myContainerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setWidget(myContainerWidget);
	setFrameShape(QFrame::NoFrame);
	setWidgetResizable(true);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

}

void ZLQtItemsListWidget::fillNodes(const ZLTreeNode *expandNode) {
	if (myLayout != 0) {
		delete myLayout;
		qDeleteAll(myContainerWidget->children());
	}
	myItems.clear();
	myLayout = new QVBoxLayout;
	myLayout->setContentsMargins(0,0,0,0);
	myLayout->setSpacing(0);
	myLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	myContainerWidget->setLayout(myLayout);

	foreach(ZLTreeNode* node, expandNode->children()) {
		if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(node)) {
			//qDebug() << QString::fromStdString(titledNode->title());
			ZLQtTreeItem *item = new ZLQtTreeItem;
			item->fill(titledNode);
			connect(item, SIGNAL(clicked(ZLQtTreeItem*)), this, SLOT(onNodeClicked(ZLQtTreeItem*))); //action ExpandAction used instead
			connect(item, SIGNAL(doubleClicked(ZLQtTreeItem*)), this, SIGNAL(nodeDoubleClicked(ZLQtTreeItem*)));
			myLayout->addWidget(item);
			myItems.push_back(item);
		}
	}

	myLayout->addStretch();
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


ZLQtTreeItem::ZLQtTreeItem(QWidget *parent) : QFrame(parent), myNode(0), myImageRequested(false) {
	setAutoFillBackground(true);
	setActive(false);

	QHBoxLayout *mainLayout = new QHBoxLayout;
	QVBoxLayout *titlesLayout = new QVBoxLayout;

	myIcon = new QLabel;
	myTitle = new QLabel;
	mySubtitle = new QLabel;

	myWaitingIcon = new ZLQtWaitingIcon;
	myWaitingIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	myTitle->setWordWrap(true);
	mySubtitle->setWordWrap(true);

	QSizePolicy policy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	//policy.setHorizontalStretch(1);
	myTitle->setSizePolicy(policy);
	mySubtitle->setSizePolicy(policy);

	titlesLayout->addWidget(myTitle);
	titlesLayout->addWidget(mySubtitle);

	mainLayout->addWidget(myIcon);
	mainLayout->addLayout(titlesLayout);
	mainLayout->addStretch();
	mainLayout->addWidget(myWaitingIcon, 1);
	setLayout(mainLayout);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setFixedHeight(ITEM_HEIGHT);

	clear();
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
		QPixmap pixmap = ZLQtImageUtils::ZLImageToQPixmapWithSize(image, QSize(ITEM_SIZE,ITEM_SIZE), Qt::SmoothTransformation);
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
	isActive = active;

	//TODO due to different themes on OS, take a color from existed palette
	QColor mainColor = isActive ? QColor::fromHsv(0, 0, 0.75 * 255) : QColor::fromHsv(0, 0, 0.95 * 255);

	setFrameStyle(QFrame::Panel | QFrame::Raised);
	setLineWidth(2);

	QPalette p = palette();
	p.setColor(QPalette::Window, mainColor);
	setPalette(p);

	update();
}

ZLTreeTitledNode *ZLQtTreeItem::getNode() const {
	return myNode;
}

ZLQtWaitingIcon *ZLQtTreeItem::getWaitingIcon() const {
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

ZLQtWaitingIcon::ZLQtWaitingIcon(QWidget* parent) : QLabel(parent), myAngle(0) {
	//TODO maybe replace to QMovie class using
	const int ICON_WIDTH = 35;
	const int ICON_HEIGHT = ICON_WIDTH;
	static std::string iconPath = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + "refresh-icon-2.png";
	//qDebug() << "LoadingIcon" << iconFile;
	myTimer = new QTimer(this);
	//TODO make pixmap as static
	myPixmap = QPixmap(ZLFile(iconPath).path().c_str());
	myPixmap = myPixmap.scaled(QSize(ICON_WIDTH, ICON_HEIGHT), Qt::KeepAspectRatio, Qt::FastTransformation);
	this->setFixedSize(myPixmap.size());
	connect(myTimer,SIGNAL(timeout()), this, SLOT(rotate()));
	this->hide();
}

//class ZLQtWaitingIconListener : public ZLNetworkRequest::Listener {
//public:
//	ZLQtWaitingIconListener(ZLQtWaitingIcon *waitingIcon) : myWaitingIcon(waitingIcon) {}
//	void finished(const std::string &/*error*/) {
//		myWaitingIcon->finish();
//	}
//private:
//	ZLQtWaitingIcon *myWaitingIcon;
//};

void ZLQtWaitingIcon::start() {
	this->show();
	const int REFRESH_TIME = 100;
	myTimer->start(REFRESH_TIME);
}

void ZLQtWaitingIcon::finish() {
	//qDebug() << Q_FUNC_INFO;
	myTimer->stop();
	this->hide();
}

void ZLQtWaitingIcon::rotate() {
	//qDebug() << this->size();
	const int ANGLE_SPEED = 360/10;
	myAngle += ANGLE_SPEED;
	if (myAngle >= 360) {
		myAngle -= 360;
	}
	QPixmap tmpPixmap(myPixmap.size());
	tmpPixmap.fill(Qt::transparent);
	QPainter painter(&tmpPixmap);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.translate(myPixmap.width()/2,myPixmap.height()/2);
	painter.rotate(qreal(myAngle));
	painter.translate(-myPixmap.width()/2,-myPixmap.height()/2);
	painter.drawPixmap(0,0,myPixmap);
	painter.end();
	this->setPixmap(tmpPixmap);
	QWidget::raise();
}
