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

#include <qwidget.h>

#include <ZLApplication.h>
#include "../../../../core/src/view/ZLViewWidget.h"

class QScrollBar;
class QFrame;
class QGridLayout;

class ZLQtApplicationWindow;

class ZLQtViewWidget : public QObject, public ZLViewWidget {
	Q_OBJECT

private:
	class ZLQtViewWidgetInternal : public QWidget {

	public:
		ZLQtViewWidgetInternal(QWidget *parent, ZLQtViewWidget &holder);

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
	ZLQtViewWidget(QWidget *parent, ZLQtApplicationWindow &applicationWindow);
	QWidget *widget() const;

private:
	void repaint();
	void trackStylus(bool track);

	void setScrollbarEnabled(ZLView::Direction direction, bool enabled);
	void setScrollbarPlacement(ZLView::Direction direction, bool standard);
	void setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to);

	QScrollBar *addScrollBar(QGridLayout *layout, Qt::Orientation orientation, int x, int y);
	QScrollBar *verticalScrollBar();
	QScrollBar *horizontalScrollBar();

private slots:
	void onVerticalSliderMoved(int value);
	void onVerticalSliderStepNext();
	void onVerticalSliderPageNext();
	void onVerticalSliderStepPrevious();
	void onVerticalSliderPagePrevious();
	void onHorizontalSliderMoved(int value);
	void onHorizontalSliderStepNext();
	void onHorizontalSliderPageNext();
	void onHorizontalSliderStepPrevious();
	void onHorizontalSliderPagePrevious();

private:
	ZLQtViewWidgetInternal *myQWidget;
	QFrame *myFrame;

	QScrollBar *myRightScrollBar;
	QScrollBar *myLeftScrollBar;
	bool myShowScrollBarAtRight;

	QScrollBar *myBottomScrollBar;
	QScrollBar *myTopScrollBar;
	bool myShowScrollBarAtBottom;

	ZLQtApplicationWindow &myApplicationWindow;

friend class ZLQtViewWidgetInternal;
};

#endif /* __ZLQTVIEWWIDGET_H__ */
