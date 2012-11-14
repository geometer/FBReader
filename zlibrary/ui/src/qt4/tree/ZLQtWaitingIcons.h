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

#ifndef __ZLQTWAITINGICONS_H__
#define __ZLQTWAITINGICONS_H__

#include <QtCore/QTimer>
#include <QtGui/QLabel>
#include <QtGui/QMovie>
#include <QtGui/QColor>

class ZLQtWaitingIcon : public QLabel {

public:
	ZLQtWaitingIcon(QWidget* parent = 0);

public:
	virtual void start() = 0;
	virtual void finish() = 0;
};

//TODO implement waiting icon based on *.png series (as a frames)

class ZLQtWaitingIconSelfRotating : public ZLQtWaitingIcon {
	Q_OBJECT

public:
	ZLQtWaitingIconSelfRotating(QSize iconSize, QWidget* parent = 0);

public Q_SLOTS:
	void start();
	void finish();

private Q_SLOTS:
	void rotate();

private:
	int myAngle;
	QPixmap myPixmap;
	QTimer* myTimer;
};

class ZLQtWaitingIconGif : public ZLQtWaitingIcon {
	Q_OBJECT

public:
	ZLQtWaitingIconGif(std::string filename, QSize iconSize, QWidget* parent = 0);

public Q_SLOTS:
	void start();
	void finish();

private:
	QMovie* myMovie;
};

class ZLQtWaitingSpinner : public ZLQtWaitingIcon {
	Q_OBJECT

public:
	explicit ZLQtWaitingSpinner(int lines = 12, int length = 7, int width = 5, int radius = 10, QWidget *parent = 0);

public Q_SLOTS:
	void start();
	void finish();

public:
	void setLines(int lines);
	void setLength(int length);
	void setWidth(int width);
	void setRadius(int radius);
	void setRoundness(qreal roundness);
	void setColor(QColor color);
	void setSpeed(qreal speed);
	void setTrail(int trail);
	void setOpacity(int minOpacity);

private Q_SLOTS:
	void rotate();
	void updateSize();
	void updateTimer();

protected:
	void paintEvent(QPaintEvent *ev);

private:
	static int countTimeout(int lines, qreal speed);
	static int lineDistance(int from, int to, int lines);
	static QColor countTrailColor(int distance, int lines, int trail, int minOpacity, QColor color);

private:
	int myLines; // The number of lines to draw
	int myLength; // The length of each line
	int myWidth; // The line thickness
	int myRadius; // The radius of the inner circle
	qreal myRoundness; //0..100
	QColor myColor;
	qreal mySpeed; // Rounds per second
	int myTrail; // Afterglow percentage
	int myOpacity; // Opacity percentage of the lines

private:
	QTimer *myTimer;
	int myCurrentCounter;
};

#endif /* __ZLQTWAITINGICONS_H__ */
