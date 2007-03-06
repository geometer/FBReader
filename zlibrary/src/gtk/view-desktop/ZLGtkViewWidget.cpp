/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ZLGtkViewWidget.h"
#include "ZLGtkPaintContext.h"
#include "../util/ZLGtkSignalUtil.h"

#include "gdk-pixbuf-hack.h"

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
	ZLPaintContext &context = viewWidget->view()->context();
	x -= context.leftMargin();
	y -= context.topMargin();
}

static void mousePressed(GtkWidget*, GdkEventButton *event, gpointer data) {
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
	gtk_widget_set_double_buffered(myArea, false);
	gtk_widget_set_events(myArea, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myArea), "button_press_event", GTK_SIGNAL_FUNC(mousePressed), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myArea), "button_release_event", GTK_SIGNAL_FUNC(mouseReleased), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myArea), "motion_notify_event", GTK_SIGNAL_FUNC(mouseMoved), this);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(myArea), "expose_event", GTK_SIGNAL_FUNC(::doPaint), this);
}

void ZLGtkViewWidget::trackStylus(bool track) {
	// TODO: implement
}

void ZLGtkViewWidget::repaint()	{
	gtk_widget_queue_draw(myArea);
}

void ZLGtkViewWidget::doPaint()	{
	ZLGtkPaintContext &gtkContext = (ZLGtkPaintContext&)view()->context();
	const int w = myArea->allocation.width;
	const int h = myArea->allocation.height;
	gtkContext.updatePixmap(myArea, w, h);
	gtkContext.setRotation(rotation());
	view()->paint();
	gdk_draw_pixmap(myArea->window, myArea->style->white_gc, gtkContext.pixmap(), 0, 0, 0, 0, w, h);
}
