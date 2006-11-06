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

#ifndef __GTKVIEWWIDGET_H__
#define __GTKVIEWWIDGET_H__

#include <gtk/gtk.h>

#include <ZLView.h>
#include <ZLApplication.h>

class GtkViewWidget : public ZLViewWidget {

public:
	GtkViewWidget(ZLApplication *application, Angle initialAngle);
	~GtkViewWidget();

	int width() const;
	int height() const;

	GtkWidget *area() { return myArea; }
	void onMousePressed(GdkEventButton *event);
	void onMouseReleased(GdkEventButton *event);
	void onMouseMoved(GdkEventMotion *event);

private:
	void trackStylus(bool track);
	void repaint();

	void cleanOriginalPixbuf();
	void cleanRotatedPixbuf();
	void updateCoordinates(int &x, int &y);

private:
	ZLApplication *myApplication;
	GtkWidget *myArea;
	GdkPixbuf *myOriginalPixbuf;
	GdkPixbuf *myRotatedPixbuf;
	GdkImage *myImage;
};

#endif /* __GTKVIEWWIDGET_H__ */
