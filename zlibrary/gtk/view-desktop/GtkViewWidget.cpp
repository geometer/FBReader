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

#include "GtkViewWidget.h"
#include "GtkPaintContext.h"

#include "gdk-pixbuf-hack.h"

static void mousePressed(GtkWidget*, GdkEventButton *event, gpointer data) {
	GtkViewWidget *viewWidget = (GtkViewWidget*)data;
	ZLView *view = viewWidget->view();
	int x, y;
	switch (viewWidget->rotation()) {
		default:
			x = (int)event->x - view->context().leftMargin(),
			y = (int)event->y - view->context().topMargin();
			break;
		case ZLViewWidget::DEGREES90:
			x = viewWidget->height() - (int)event->y - view->context().rightMargin(),
			y = (int)event->x - view->context().topMargin();
			break;
		case ZLViewWidget::DEGREES180:
			x = viewWidget->width() - (int)event->x - view->context().rightMargin(),
			y = viewWidget->height() - (int)event->y - view->context().bottomMargin();
			break;
		case ZLViewWidget::DEGREES270:
			x = (int)event->y - view->context().leftMargin();
			y = viewWidget->width() - (int)event->x - view->context().bottomMargin();
			break;
	}
	view->onStylusPress(x, y);
}

int GtkViewWidget::width() const {
	return (myArea != 0) ? myArea->allocation.width : 0;
}

int GtkViewWidget::height() const {
	return (myArea != 0) ? myArea->allocation.height : 0;
}

GtkViewWidget::GtkViewWidget(ZLApplication *application, Angle initialAngle) : ZLViewWidget(initialAngle) {
	myApplication = application;
	myArea = gtk_drawing_area_new();
	gtk_widget_set_double_buffered(myArea, false);
	gtk_widget_set_events(myArea, GDK_BUTTON_PRESS_MASK);
	gtk_signal_connect(GTK_OBJECT(myArea), "button_press_event", GTK_SIGNAL_FUNC(mousePressed), this);
}

GtkViewWidget::~GtkViewWidget() {
}

void GtkViewWidget::trackStylus(bool track) {
	// TODO: implement
}

void GtkViewWidget::repaintView()	{
	GtkPaintContext &gtkContext = (GtkPaintContext&)view()->context();
	const int w = myArea->allocation.width;
	const int h = myArea->allocation.height;
	gtkContext.updatePixmap(myArea, w, h);
	gtkContext.setRotation(rotation());
	view()->paint();
	gdk_draw_pixmap(myArea->window, myArea->style->white_gc, gtkContext.pixmap(), 0, 0, 0, 0, w, h);
	myApplication->refreshWindow();
}
