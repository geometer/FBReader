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

#include "GtkViewWidget.h"
#include "GtkFBReader.h"
#include "GtkPaintContext.h"

static void mousePressed(GtkWidget*, GdkEventButton *event, gpointer data) {
	View *view = ((GtkViewWidget*)data)->view();	
	view->onStylusPress(
		(int)event->x - view->context().leftMargin().value(),
		(int)event->y - view->context().topMargin().value());
}

GtkViewWidget::GtkViewWidget(GtkFBReader *reader) {
	myReader = reader;
	myArea = gtk_drawing_area_new();
	gtk_widget_set_events(myArea, GDK_BUTTON_PRESS_MASK);
	gtk_signal_connect(GTK_OBJECT(myArea), "button_press_event", GTK_SIGNAL_FUNC(mousePressed), this);
}

void GtkViewWidget::repaintView()	{
	GtkPaintContext &gtkContext = (GtkPaintContext&)view()->context();
	gtkContext.updatePixmap(myArea);
	view()->paint();
	gdk_draw_pixmap(myArea->window, myArea->style->white_gc, gtkContext.pixmap(), 0, 0, 0, 0, myArea->allocation.width, myArea->allocation.height);
	myReader->enableMenuButtons();
}
