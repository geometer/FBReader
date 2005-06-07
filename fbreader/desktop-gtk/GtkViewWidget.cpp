/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <iostream>
#include <abstract/ZLTime.h>

#include "GtkViewWidget.h"
#include "GtkFBReader.h"
#include "GtkPaintContext.h"

#include "gdk-pixbuf-hack.h"

static void mousePressed(GtkWidget*, GdkEventButton *event, gpointer data) {
	GtkViewWidget *viewWidget = (GtkViewWidget*)data;
	View *view = viewWidget->view();
	if (viewWidget->isRotated()) {
		view->onStylusPress(
			viewWidget->height() - (int)event->y - view->context().rightMargin().value(),
			(int)event->x - view->context().topMargin().value());
	} else {
		view->onStylusPress(
			(int)event->x - view->context().leftMargin().value(),
			(int)event->y - view->context().topMargin().value());
	}
}

int GtkViewWidget::height() const {
	return (myArea != 0) ? myArea->allocation.height : 0;
}

GtkViewWidget::GtkViewWidget(GtkFBReader *reader) {
	myReader = reader;
	myArea = gtk_drawing_area_new();
	myOriginalPixbuf = 0;
	gtk_widget_set_double_buffered(myArea, false);
	gtk_widget_set_events(myArea, GDK_BUTTON_PRESS_MASK);
	gtk_signal_connect(GTK_OBJECT(myArea), "button_press_event", GTK_SIGNAL_FUNC(mousePressed), this);
}

GtkViewWidget::~GtkViewWidget() {
	if (myOriginalPixbuf != 0) {
		gdk_pixbuf_unref(myOriginalPixbuf);
		gdk_pixbuf_unref(myRotatedPixbuf);
		gdk_image_unref(myImage);
	}
}

void GtkViewWidget::repaintView()	{
  const ZLTime c0;
	GtkPaintContext &gtkContext = (GtkPaintContext&)view()->context();
	const int w = myArea->allocation.width;
	const int h = myArea->allocation.height;
	gtkContext.updatePixmap(myArea, w, h);
	gtkContext.setRotation(isRotated());
  const ZLTime c1;
	view()->paint();
  const ZLTime c2;
	gdk_draw_pixmap(myArea->window, myArea->style->white_gc, gtkContext.pixmap(), 0, 0, 0, 0, w, h);
	const ZLTime c3;

  std::cout <<
    c1.millisecondsFrom(c0) << " + " <<
    c2.millisecondsFrom(c1) << " + " <<
    c3.millisecondsFrom(c2) << " = " <<
    c3.millisecondsFrom(c0) << std::endl;
	
	myReader->enableMenuButtons();
}
