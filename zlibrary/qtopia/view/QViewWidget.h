/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __QVIEWWIDGET_H__
#define __QVIEWWIDGET_H__

#include <qwidget.h>

#include <abstract/ZLView.h>

class QViewWidget : public QWidget, public ZLViewWidget {

public:
	QViewWidget(QWidget *parent, ZLApplication *application, Angle initialAngle);
	~QViewWidget() {}

protected:
	void repaintView();
	void trackStylus(bool track);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	int x(const QMouseEvent *event) const;
	int y(const QMouseEvent *event) const;

private:
	ZLApplication *myApplication;
};

#endif /* __QVIEWWIDGET_H__ */
