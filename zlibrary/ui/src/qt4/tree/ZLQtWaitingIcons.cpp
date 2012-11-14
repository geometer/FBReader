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

ZLQtWaitingSpinner::ZLQtWaitingSpinner(int lines, int length, int width, int radius, QWidget *parent) : ZLQtWaitingIcon(parent),
	myLines(lines),
	myLength(length + width),
	myWidth(width),
	myRadius(radius),
	myRoundness(70.0),
	myColor(Qt::black),
	mySpeed(1),
	myTrail(70),
	myOpacity(15)
{
	myCurrentCounter = 0;
	myTimer = new QTimer(this);
	connect(myTimer,SIGNAL(timeout()), this, SLOT(rotate()));
	updateSize();
	updateTimer();
	this->hide();
}

void ZLQtWaitingSpinner::paintEvent(QPaintEvent */*ev*/) {
	QPainter painter(this);
	painter.fillRect(this->rect(), Qt::transparent);
	painter.setRenderHint(QPainter::Antialiasing, true);

	if (myCurrentCounter >= myLines) {
		myCurrentCounter = 0;
	}
	painter.setPen(Qt::NoPen);
	for (int i = 0; i < myLines; ++i) {
		painter.save();
		painter.translate(myRadius + myLength, myRadius + myLength);
		qreal rotateAngle = (qreal)360 * qreal(i) / qreal(myLines);
		painter.rotate(rotateAngle);
		painter.translate(myRadius, 0);
		int distance = lineDistance(i, myCurrentCounter, myLines);
		QColor color = countTrailColor(distance, myLines, myTrail, myOpacity, myColor);
		painter.setBrush(color);
		//painter.fillRect(QRect(0, -myWidth/2, myLength, myWidth), color);
		//TODO make rect with circles
		painter.drawRoundedRect(QRect(0, -myWidth/2, myLength, myWidth), myRoundness, myRoundness, Qt::RelativeSize);
		painter.restore();
	}
}

void ZLQtWaitingSpinner::start() {
	this->show();
	myTimer->start();
	myCurrentCounter = 0;
}

void ZLQtWaitingSpinner::finish() {
	myTimer->stop();
	myCurrentCounter = 0;
	this->hide();
}

void ZLQtWaitingSpinner::setLines(int lines) {
	myLines = lines;
	myCurrentCounter = 0;
	updateTimer();
}

void ZLQtWaitingSpinner::setLength(int length){
	myLength = length;
	updateSize();
}

void ZLQtWaitingSpinner::setWidth(int width) {
	myWidth = width;
	updateSize();
}

void ZLQtWaitingSpinner::setRadius(int radius) {
	myRadius = radius;
	updateSize();
}

void ZLQtWaitingSpinner::setRoundness(qreal roundness) {
	myRoundness = std::max(0.0, std::min(100.0, roundness));
}

void ZLQtWaitingSpinner::setColor(QColor color) {
	myColor = color;
}

void ZLQtWaitingSpinner::setSpeed(qreal speed) {
	mySpeed = speed;
	updateTimer();
}

void ZLQtWaitingSpinner::setTrail(int trail) {
	myTrail = trail;
}

void ZLQtWaitingSpinner::setOpacity(int minOpacity) {
	myOpacity = minOpacity;
}

void ZLQtWaitingSpinner::rotate() {
	++myCurrentCounter;
	if (myCurrentCounter >= myLines) {
		myCurrentCounter = 0;
	}
	update();
}

void ZLQtWaitingSpinner::updateSize() {
	int size = (myRadius + myLength) * 2;
	setFixedSize(size, size);
}

void ZLQtWaitingSpinner::updateTimer() {
	myTimer->setInterval(countTimeout(myLines, mySpeed));
}

int ZLQtWaitingSpinner::countTimeout(int lines, qreal speed) {
	return 1000 / (lines * speed);
}

int ZLQtWaitingSpinner::lineDistance(int from, int to, int lines) {
	int result = to - from;
	if (result < 0) {
		result += lines;
	}
	return result;
}

QColor ZLQtWaitingSpinner::countTrailColor(int distance, int lines, int trail, int minOpacity, QColor color) {
	if (distance == 0) {
		return color;
	}
	const qreal minAlphaF = (qreal)minOpacity / 100;
	int distanceThreshold = ceil( (lines - 1) * (qreal)trail / 100);
	if (distance > distanceThreshold) {
		color.setAlphaF(minAlphaF);
		return color;
	}
	qreal alphaDiff = color.alphaF() - (qreal)minAlphaF;
	qreal gradation = alphaDiff / (qreal)(distanceThreshold + 1);
	qreal resultAlpha = color.alphaF() - gradation * distance;
	resultAlpha = std::min(1.0, std::max(0.0, resultAlpha)); //if alpha is out of bound, force it to bounds
	color.setAlphaF(resultAlpha);
	return color;
}
