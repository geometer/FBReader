/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTVIEWWIDGET_H__
#define __ZLQTVIEWWIDGET_H__

#include <qwidget.h>

#include <ZLView.h>
#include <ZLApplication.h>

class ZLQtApplicationWindow;

class ZLQtViewWidgetPositionInfo {

public:
	ZLQtViewWidgetPositionInfo(const ZLQtApplicationWindow &window);

	int x() const;
	int y() const;
	int width() const;
	int height() const;

private:
	const ZLQtApplicationWindow &myWindow;
};

class ZLQtViewWidget : public ZLViewWidget {

private:
	class ZLQtViewWidgetInternal : public QWidget {

	public:
		ZLQtViewWidgetInternal(QWidget *parent, ZLQtViewWidget &holder);

	private:
		void paintEvent(QPaintEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void resizeEvent(QResizeEvent *event);

		int x(const QMouseEvent *event) const;
		int y(const QMouseEvent *event) const;

	private:
		ZLQtViewWidget &myHolder;
	};
	
public:
	ZLQtViewWidget(QWidget *parent, ZLApplication *application, const ZLQtViewWidgetPositionInfo &positionInfo);
	QWidget *widget();
	const ZLQtViewWidgetPositionInfo &positionInfo() const;

private:
	void repaint();
	void trackStylus(bool track);

private:
	ZLQtViewWidgetInternal *myQWidget;
	ZLApplication *myApplication;
	const ZLQtViewWidgetPositionInfo myPositionInfo;
};

inline QWidget *ZLQtViewWidget::widget() { return myQWidget; }

#endif /* __ZLQTVIEWWIDGET_H__ */
