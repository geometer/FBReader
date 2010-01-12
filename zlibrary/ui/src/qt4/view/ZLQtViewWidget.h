/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <QtGui/QWidget>

#include "../../../../core/src/view/ZLViewWidget.h"
#include <ZLApplication.h>

class QGridLayout;
class QScrollBar;

class ZLQtViewWidget : public QObject, public ZLViewWidget {
	Q_OBJECT

private:
	class Widget : public QWidget {

	public:
		Widget(QWidget *parent, ZLQtViewWidget &holder);

	private:
		void paintEvent(QPaintEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

		int x(const QMouseEvent *event) const;
		int y(const QMouseEvent *event) const;

	private:
		ZLQtViewWidget &myHolder;
	};
	
public:
	ZLQtViewWidget(QWidget *parent, ZLApplication *application);
	QWidget *widget();

private Q_SLOTS:
	void onVerticalSliderMoved(int value);
	void onHorizontalSliderMoved(int value);
	void onVerticalSliderClicked(int value);
	void onHorizontalSliderClicked(int value);

private:
	void repaint();
	void trackStylus(bool track);

	void setScrollbarEnabled(ZLView::Direction direction, bool enabled);
	void setScrollbarPlacement(ZLView::Direction direction, bool standard);
	void setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to);

	QScrollBar *addScrollBar(QGridLayout *layout, Qt::Orientation orientation, int x, int y);

private:
	QWidget *myFrame;
	Widget *myQWidget;

	QScrollBar *myRightScrollBar;
	QScrollBar *myLeftScrollBar;
	bool myShowScrollBarAtRight;

	QScrollBar *myBottomScrollBar;
	QScrollBar *myTopScrollBar;
	bool myShowScrollBarAtBottom;

	ZLApplication *myApplication;
};

#endif /* __ZLQTVIEWWIDGET_H__ */
