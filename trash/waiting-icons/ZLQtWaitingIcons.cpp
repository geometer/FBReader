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

#include <cmath>
#include <algorithm>

#include <QtGui/QPainter>

#include <ZLibrary.h>
#include <ZLFile.h>

#include "ZLQtWaitingIcons.h"

ZLQtWaitingIcon::ZLQtWaitingIcon(QWidget *parent) : QLabel(parent) {
}

ZLQtWaitingIconSelfRotating::ZLQtWaitingIconSelfRotating(QSize iconSize, QWidget* parent) : ZLQtWaitingIcon(parent), myAngle(0) {
	//TODO maybe replace to QMovie class using
	const int iconWidth = iconSize.width();
	const int iconHeight = iconSize.height();
	static std::string iconPath = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + "refresh-icon-2.png";
	//qDebug() << "LoadingIcon" << iconFile;
	myTimer = new QTimer(this);
	//TODO make pixmap as static
	myPixmap = QPixmap(ZLFile(iconPath).path().c_str());
	myPixmap = myPixmap.scaled(QSize(iconWidth, iconHeight), Qt::KeepAspectRatio, Qt::FastTransformation);
	this->setFixedSize(myPixmap.size());
	connect(myTimer,SIGNAL(timeout()), this, SLOT(rotate()));
	this->hide();
}

//class ZLQtWaitingIconSelfRotatingListener : public ZLNetworkRequest::Listener {
//public:
//	ZLQtWaitingIconSelfRotatingListener(ZLQtWaitingIconSelfRotating *waitingIcon) : myWaitingIcon(waitingIcon) {}
//	void finished(const std::string &/*error*/) {
//		myWaitingIcon->finish();
//	}
//private:
//	ZLQtWaitingIconSelfRotating *myWaitingIcon;
//};

void ZLQtWaitingIconSelfRotating::start() {
	this->show();
	const int REFRESH_TIME = 100;
	myTimer->start(REFRESH_TIME);
}

void ZLQtWaitingIconSelfRotating::finish() {
	//qDebug() << Q_FUNC_INFO;
	myTimer->stop();
	this->hide();
}

void ZLQtWaitingIconSelfRotating::rotate() {
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

ZLQtWaitingIconGif::ZLQtWaitingIconGif(std::string filename, QSize iconSize, QWidget *parent) : ZLQtWaitingIcon(parent) {
	static std::string iconPath = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + filename;
	myMovie = new QMovie(this);
	myMovie->setFileName(QString::fromStdString(ZLFile(iconPath).path()));
	myMovie->setBackgroundColor(Qt::transparent);
	this->setFixedSize(iconSize);
	this->setMovie(myMovie);
}

void ZLQtWaitingIconGif::start() {
	this->show();
	myMovie->start();
}

void ZLQtWaitingIconGif::finish() {
	myMovie->stop();
	this->hide();
}
