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
		case ZLViewWidget::DEGREES90:
		{
			int tmp = x;
			x = viewWidget->height() - y;
			y = tmp;
			break;
		}
		case ZLViewWidget::DEGREES180:
			x = viewWidget->width() - x;
			y = viewWidget->height() - y;
			break;
		case ZLViewWidget::DEGREES270:
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
	if (state == 0) {
		viewWidget->view()->onStylusMove(x, y);
	} else {
		viewWidget->view()->onStylusMovePressed(x, y);
	}
}

static void doPaint(GtkWidget*, GdkEventExpose *event, gpointer data) {
	((ZLGtkViewWidget*)data)->doPaint();
}

int ZLGtkViewWidget::width() const {
	return (myArea != 0) ? myArea->allocation.width : 0;
}

int ZLGtkViewWidget::height() const {
	return (myArea != 0) ? myArea->allocation.height : 0;
}

ZLGtkViewWidget::ZLGtkViewWidget(ZLApplication *application, Angle initialAngle) : ZLViewWidget(initialAngle) {
	myApplication = application;
	myArea = gtk_drawing_area_new();
	GTK_OBJECT_SET_FLAGS(myArea, GTK_CAN_FOCUS);
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
	Angle angle = rotation();
	bool isRotated = (angle == DEGREES90) || (angle == DEGREES270);
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
		case DEGREES180:
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
		case DEGREES90:
		case DEGREES270:
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
			gdk_pixbuf_get_from_image(myOriginalPixbuf, myImage, gdk_drawable_get_colormap(gtkContext.pixmap()),
																0, 0, 0, 0, w, h);
			::rotate90(myRotatedPixbuf, myOriginalPixbuf, angle == DEGREES90);
			gdk_draw_pixbuf(myArea->window, myArea->style->white_gc, myRotatedPixbuf,
											0, 0, 0, 0, h, w, GDK_RGB_DITHER_NONE, 0, 0);
			break;
	}
	myRepaintBlocked = true;
	myApplication->refreshWindow();
	myRepaintBlocked = false;
}
