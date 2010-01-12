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

#ifndef __ZLGTKVIEWWIDGET_H__
#define __ZLGTKVIEWWIDGET_H__

#include <gtk/gtk.h>

#include "../../../../core/src/view/ZLViewWidget.h"
#include <ZLApplication.h>

class ZLGtkViewWidget : public ZLViewWidget {

public:
	ZLGtkViewWidget(ZLApplication *application, ZLView::Angle initialAngle);
	~ZLGtkViewWidget();

	int width() const;
	int height() const;
	void doPaint();

	GtkWidget *area();
	GtkWidget *areaWithScrollbars();

	bool scrollbarEvent(ZLView::Direction direction, GtkRange *range, GtkScrollType type, double newValue);

private:
	void trackStylus(bool track);
	void repaint();

	GtkWidget *createVScrollbar(int pos);
	GtkWidget *createHScrollbar(int pos);

	void setScrollbarEnabled(ZLView::Direction direction, bool enabled);
	void setScrollbarPlacement(ZLView::Direction direction, bool standard);
	void setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to);

	void cleanOriginalPixbuf();
	void cleanRotatedPixbuf();

private:
	ZLApplication *myApplication;
	GtkWidget *myArea;
	GtkTable *myTable;
	GdkPixbuf *myOriginalPixbuf;
	GdkPixbuf *myRotatedPixbuf;
	GdkImage *myImage;
	bool myRepaintBlocked;

	GtkAdjustment *myVerticalAdjustment;
	GtkWidget *myRightScrollBar;
	GtkWidget *myLeftScrollBar;
	bool myShowScrollBarAtRight;
	bool myVerticalScrollbarIsVisible;

	GtkAdjustment *myHorizontalAdjustment;
	GtkWidget *myBottomScrollBar;
	GtkWidget *myTopScrollBar;
	bool myShowScrollBarAtBottom;
	bool myHorizontalScrollbarIsVisible;
};

#endif /* __ZLGTKVIEWWIDGET_H__ */
