#include <cmath>
#include <algorithm>

#include <QtGui/QPainter>

#include "QtWaitingSpinner.h"

QtWaitingSpinner::QtWaitingSpinner(int linesNumber, int length, int width, int radius, QWidget *parent) : QWidget(parent),
	myLinesNumber(linesNumber),
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

void QtWaitingSpinner::paintEvent(QPaintEvent */*ev*/) {
	QPainter painter(this);
	painter.fillRect(this->rect(), Qt::transparent);
	painter.setRenderHint(QPainter::Antialiasing, true);

	if (myCurrentCounter >= myLinesNumber) {
		myCurrentCounter = 0;
	}
	painter.setPen(Qt::NoPen);
	for (int i = 0; i < myLinesNumber; ++i) {
		painter.save();
		painter.translate(myRadius + myLength, myRadius + myLength);
		qreal rotateAngle = (qreal)360 * qreal(i) / qreal(myLinesNumber);
		painter.rotate(rotateAngle);
		painter.translate(myRadius, 0);
		int distance = lineDistance(i, myCurrentCounter, myLinesNumber);
		QColor color = countTrailColor(distance, myLinesNumber, myTrail, myOpacity, myColor);
		painter.setBrush(color);
		//TODO improve the way rounded rect is painted
		painter.drawRoundedRect(QRect(0, -myWidth/2, myLength, myWidth), myRoundness, myRoundness, Qt::RelativeSize);
		painter.restore();
	}
}

void QtWaitingSpinner::start() {
	this->show();
	if (!myTimer->isActive()) {
		myTimer->start();
		myCurrentCounter = 0;
	}
}

void QtWaitingSpinner::finish() {
	this->hide();
	if (myTimer->isActive()) {
		myTimer->stop();
		myCurrentCounter = 0;
	}
}

void QtWaitingSpinner::setLinesNumber(int linesNumber) {
	myLinesNumber = linesNumber;
	myCurrentCounter = 0;
	updateTimer();
}

void QtWaitingSpinner::setLength(int length){
	myLength = length;
	updateSize();
}

void QtWaitingSpinner::setWidth(int width) {
	myWidth = width;
	updateSize();
}

void QtWaitingSpinner::setRadius(int radius) {
	myRadius = radius;
	updateSize();
}

void QtWaitingSpinner::setRoundness(qreal roundness) {
	myRoundness = std::max((qreal)0.0, std::min((qreal)100.0, roundness));
}

void QtWaitingSpinner::setColor(QColor color) {
	myColor = color;
}

void QtWaitingSpinner::setSpeed(qreal speed) {
	mySpeed = speed;
	updateTimer();
}

void QtWaitingSpinner::setTrail(int trail) {
	myTrail = trail;
}

void QtWaitingSpinner::setOpacity(int minOpacity) {
	myOpacity = minOpacity;
}

void QtWaitingSpinner::rotate() {
	++myCurrentCounter;
	if (myCurrentCounter >= myLinesNumber) {
		myCurrentCounter = 0;
	}
	update();
}

void QtWaitingSpinner::updateSize() {
	int size = (myRadius + myLength) * 2;
	setFixedSize(size, size);
}

void QtWaitingSpinner::updateTimer() {
	myTimer->setInterval(countTimeout(myLinesNumber, mySpeed));
}

int QtWaitingSpinner::countTimeout(int lines, qreal speed) {
	return 1000 / (lines * speed);
}

int QtWaitingSpinner::lineDistance(int from, int to, int lines) {
	int result = to - from;
	if (result < 0) {
		result += lines;
	}
	return result;
}

QColor QtWaitingSpinner::countTrailColor(int distance, int lines, int trail, int minOpacity, QColor color) {
	if (distance == 0) {
		return color;
	}
	const qreal minAlphaF = (qreal)minOpacity / 100;
	int distanceThreshold = std::ceil( (lines - 1) * (qreal)trail / 100);
	if (distance > distanceThreshold) {
		color.setAlphaF(minAlphaF);
		return color;
	}
	qreal alphaDiff = color.alphaF() - (qreal)minAlphaF;
	qreal gradation = alphaDiff / (qreal)(distanceThreshold + 1);
	qreal resultAlpha = color.alphaF() - gradation * distance;
	//if alpha is out of bound, force it to bounds
	resultAlpha = std::min((qreal)1.0, std::max((qreal)0.0, resultAlpha));
	color.setAlphaF(resultAlpha);
	return color;
}

