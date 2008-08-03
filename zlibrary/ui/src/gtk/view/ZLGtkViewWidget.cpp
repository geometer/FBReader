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

#include <gtk/gtkwindow.h>

#include "ZLGtkViewWidget.h"
#include "ZLGtkPaintContext.h"
#include "../util/ZLGtkSignalUtil.h"
#include "../pixbuf/ZLGtkPixbufHack.h"

static void updatePoint(ZLGtkViewWidget *viewWidget, int &x, int &y) {
	switch (viewWidget->rotation()) {
		default:
			break;
		case ZLView::DEGREES90:
		{
			int tmp = x;
			x = viewWidget->height() - y;
			y = tmp;
			break;
		}
		case ZLView::DEGREES180:
			x = viewWidget->width() - x;
			y = viewWidget->height() - y;
			break;
		case ZLView::DEGREES270:
		{
			int tmp = x;
			x = y;
			y = viewWidget->width() - tmp;
			break;
		}
	}
}

static void mousePressed(GtkWidget *area, GdkEventButton *event, gpointer data) {
	gtk_window_set_focus(GTK_WINDOW(gtk_widget_get_toplevel(area)), area);
	ZLGtkViewWidget *viewWidget = (ZLGtkViewWidget*)data;
	int x = (int)event->x;
	int y = (int)event->y;
	updatePoint(viewWidget, x, y);
	viewWidget->view()->onStylusMove(x, y);
	viewWidget->view()->onStylusPress(x, y);
}

static void mouseReleased(GtkWidget*, GdkEventButton *event, gpointer data) {
	ZLGtkViewWidget *viewWidget = (ZLGtkViewWidget*)data;
	int x = (int)event->x;
	int y = (int)event->y;
	updatePoint(viewWidget, x, y);
	viewWidget->view()->onStylusRelease(x, y);
}

static void mouseMoved(GtkWidget*, GdkEventMotion *event, gpointer data) {
	ZLGtkViewWidget *viewWidget = (ZLGtkViewWidget*)data;
	int x, y;
	GdkModifierType state;
	if (event->is_hint) {
		gdk_window_get_pointer(event->window, &x, &y, &state);
	} else {
		x = (int)event->x;
		y = (int)event->y;
		state = (GdkModifierType)event->state;
	}
	updatePoint(viewWidget, x, y);
	if ((state & 0xFF00) == 0) {
		viewWidget->view()->onStylusMove(x, y);
	} else {
		viewWidget->view()->onStylusMovePressed(x, y);
	}
}

static void doPaint(GtkWidget*, GdkEventExpose*, ZLGtkViewWidget *data) {
	data->doPaint();
}

int ZLGtkViewWidget::width() const {
	return (myArea != 0) ? myArea->allocation.width : 0;
}

int ZLGtkViewWidget::height() const {
	return (myArea != 0) ? myArea->allocation.height : 0;
}

static void vScrollbarMoved(GtkAdjustment *adjustment, ZLGtkViewWidget *data) {
	data->onScrollbarMoved(
		ZLView::VERTICAL,
		(size_t)adjustment->upper,
		(size_t)adjustment->value,
		(size_t)(adjustment->value + adjustment->page_size)
	);
}

static void hScrollbarMoved(GtkAdjustment *adjustment, ZLGtkViewWidget *data) {
	data->onScrollbarMoved(
		ZLView::HORIZONTAL,
		(size_t)adjustment->upper,
		(size_t)adjustment->value,
		(size_t)(adjustment->value + adjustment->page_size)
	);
}

ZLGtkViewWidget::ZLGtkViewWidget(ZLApplication *application, ZLView::Angle initialAngle) : ZLViewWidget(initialAngle) {
	myApplication = application;
	myArea = gtk_drawing_area_new();
	GTK_OBJECT_SET_FLAGS(myArea, GTK_CAN_FOCUS);

	myScrollArea = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(0, 0));
	GtkAdjustment *hAdjustment = gtk_scrolled_window_get_hadjustment(myScrollArea);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(hAdjustment), "value_changed", GTK_SIGNAL_FUNC(hScrollbarMoved), this);
	GtkAdjustment *vAdjustment = gtk_scrolled_window_get_vadjustment(myScrollArea);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(vAdjustment), "value_changed", GTK_SIGNAL_FUNC(vScrollbarMoved), this);
	gtk_container_add(GTK_CONTAINER(myScrollArea), myArea);
	//gtk_scrolled_window_add_with_viewport(myScrollArea, myArea);
	setScrollbarEnabled(ZLView::VERTICAL, false);
	setScrollbarEnabled(ZLView::HORIZONTAL, false);

	myOriginalPixbuf = 0;
	myRotatedPixbuf = 0;
	gtk_widget_set_double_buffered(myArea, false);
	gtk_widget_set_events(myArea, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myArea), "button_press_event", GTK_SIGNAL_FUNC(mousePressed), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myArea), "button_release_event", GTK_SIGNAL_FUNC(mouseReleased), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myArea), "motion_notify_event", GTK_SIGNAL_FUNC(mouseMoved), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myArea), "expose_event", GTK_SIGNAL_FUNC(::doPaint), this);
	myRepaintBlocked = false;
}

void ZLGtkViewWidget::setScrollbarEnabled(ZLView::Direction direction, bool enabled) {
	const bool hEnabled = (direction == ZLView::HORIZONTAL) && enabled;
	const bool vEnabled = (direction == ZLView::VERTICAL) && enabled;
	gtk_scrolled_window_set_policy(myScrollArea, hEnabled ? GTK_POLICY_ALWAYS : GTK_POLICY_NEVER, vEnabled ? GTK_POLICY_ALWAYS : GTK_POLICY_NEVER);
	if (hEnabled) {
		gtk_widget_queue_draw(gtk_scrolled_window_get_hscrollbar(myScrollArea));
	}
	if (vEnabled) {
		gtk_widget_queue_draw(gtk_scrolled_window_get_vscrollbar(myScrollArea));
	}
}

void ZLGtkViewWidget::setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to, size_t step) {
	GtkAdjustment *adjustment =
		(direction == ZLView::VERTICAL) ?
			gtk_scrolled_window_get_vadjustment(myScrollArea) :
			gtk_scrolled_window_get_hadjustment(myScrollArea);
	adjustment->lower = 0;
	adjustment->upper = full;
	adjustment->value = from;
	adjustment->step_increment = step;
	adjustment->page_increment = step;
	adjustment->page_size = to - from;
	if (direction == ZLView::VERTICAL) {
		gtk_scrolled_window_set_vadjustment(myScrollArea, adjustment);
		gtk_widget_queue_draw(gtk_scrolled_window_get_vscrollbar(myScrollArea));
	} else {
		gtk_scrolled_window_set_hadjustment(myScrollArea, adjustment);
		gtk_widget_queue_draw(gtk_scrolled_window_get_hscrollbar(myScrollArea));
	}
}

ZLGtkViewWidget::~ZLGtkViewWidget() {
	cleanOriginalPixbuf();
	cleanRotatedPixbuf();
}

void ZLGtkViewWidget::cleanOriginalPixbuf() {
	if (myOriginalPixbuf != 0) {
		gdk_pixbuf_unref(myOriginalPixbuf);
		gdk_image_unref(myImage);
		myOriginalPixbuf = 0;
	}
}

void ZLGtkViewWidget::cleanRotatedPixbuf() {
	if (myRotatedPixbuf != 0) {
		gdk_pixbuf_unref(myRotatedPixbuf);
		myRotatedPixbuf = 0;
	}
}

void ZLGtkViewWidget::trackStylus(bool track) {
	// TODO: implement
}

void ZLGtkViewWidget::repaint()	{
	if (!myRepaintBlocked) {
		gtk_widget_queue_draw(myArea);
	}
}

void ZLGtkViewWidget::doPaint()	{
	ZLGtkPaintContext &gtkContext = (ZLGtkPaintContext&)view()->context();
	ZLView::Angle angle = rotation();
	bool isRotated = (angle == ZLView::DEGREES90) || (angle == ZLView::DEGREES270);
	int w = isRotated ? myArea->allocation.height : myArea->allocation.width;
	int h = isRotated ? myArea->allocation.width : myArea->allocation.height;
	gtkContext.updatePixmap(myArea, w, h);
	view()->paint();
	switch (angle) {
		default:
			cleanOriginalPixbuf();
			cleanRotatedPixbuf();
			gdk_draw_pixmap(myArea->window, myArea->style->white_gc, gtkContext.pixmap(),
											0, 0, 0, 0, myArea->allocation.width, myArea->allocation.height);
			break;
		case ZLView::DEGREES180:
			cleanRotatedPixbuf();
			if ((myOriginalPixbuf != 0) &&
					((gdk_pixbuf_get_width(myOriginalPixbuf) != w) ||
					 (gdk_pixbuf_get_height(myOriginalPixbuf) != h))) {
				cleanOriginalPixbuf();
			}
			if (myOriginalPixbuf == 0) {
				myOriginalPixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, w, h);
				myImage = gdk_image_new(GDK_IMAGE_FASTEST, gdk_drawable_get_visual(gtkContext.pixmap()), w, h);
			}
			gdk_drawable_copy_to_image(gtkContext.pixmap(), myImage, 0, 0, 0, 0, w, h);
			gdk_pixbuf_get_from_image(myOriginalPixbuf, myImage, gdk_drawable_get_colormap(gtkContext.pixmap()),
																0, 0, 0, 0, w, h);
			::rotate180(myOriginalPixbuf);
			gdk_draw_pixbuf(myArea->window, myArea->style->white_gc, myOriginalPixbuf,
											0, 0, 0, 0, w, h, GDK_RGB_DITHER_NONE, 0, 0);
			break;
		case ZLView::DEGREES90:
		case ZLView::DEGREES270:
			if ((myOriginalPixbuf != 0) &&
					((gdk_pixbuf_get_width(myOriginalPixbuf) != w) ||
					 (gdk_pixbuf_get_height(myOriginalPixbuf) != h))) {
				cleanOriginalPixbuf();
			}
			if ((myRotatedPixbuf != 0) &&
					((gdk_pixbuf_get_width(myRotatedPixbuf) != h) ||
					 (gdk_pixbuf_get_height(myRotatedPixbuf) != w))) {
				cleanRotatedPixbuf();
			}
			if (myOriginalPixbuf == 0) {
				myOriginalPixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, w, h);
				myImage = gdk_image_new(GDK_IMAGE_FASTEST, gdk_drawable_get_visual(gtkContext.pixmap()), w, h);
			}
			if (myRotatedPixbuf == 0) {
				myRotatedPixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, h, w);
			}
			gdk_drawable_copy_to_image(gtkContext.pixmap(), myImage, 0, 0, 0, 0, w, h);
			gdk_pixbuf_get_from_image(myOriginalPixbuf, myImage, gdk_drawable_get_colormap(gtkContext.pixmap()), 0, 0, 0, 0, w, h);
			::rotate90(myRotatedPixbuf, myOriginalPixbuf, angle == ZLView::DEGREES90);
			gdk_draw_pixbuf(myArea->window, myArea->style->white_gc, myRotatedPixbuf, 0, 0, 0, 0, h, w, GDK_RGB_DITHER_NONE, 0, 0);
			break;
	}
	myRepaintBlocked = true;
	myApplication->refreshWindow();
	myRepaintBlocked = false;
}

GtkWidget *ZLGtkViewWidget::area() {
	return myArea;
}

GtkWidget *ZLGtkViewWidget::areaWithScrollbar() {
	return GTK_WIDGET(myScrollArea);
}
