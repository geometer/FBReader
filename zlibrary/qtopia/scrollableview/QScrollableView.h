/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __QSCROLLABLEVIEW_H__
#define __QSCROLLABLEVIEW_H__

#include <qwidget.h>

#include "../../abstract/scrollableview/ZLScrollableView.h"

class QPainter;
class QScrollView;
class QPixmap;
class QTimer;

class QScrollableView : public QWidget, public ZLScrollableView {

Q_OBJECT

public:
	QScrollableView(QWidget *parent);
	~QScrollableView();
	QScrollView &scrollWidget() const { return *myScrollWidget; }
	QPixmap &doubleBufferPixmap() const { return *myDoubleBufferPixmap; }
	
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);

	void repaintView();
	void clearView();
	void paintEvent(QPaintEvent *event);

	int currentWidth() const { return width(); }
	int currentHeight() const { return height(); }

	void startTimer(int mseconds);

	int visibleAreaWidth(bool scrollBarHidden) const;
	int visibleAreaHeight(bool scrollBarHidden) const;
	void resizeView(int w, int h);

private slots:
	void onTimerTick();

private:
	QScrollView *myScrollWidget;
	bool myDoRealRepaint;
	QPixmap *myDoubleBufferPixmap;
	QTimer *myTimer;
};

#endif /* __QSCROLLABLEVIEW_H__ */
