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
#endif /* __ZLQTWAITINGICONS_H__ */
