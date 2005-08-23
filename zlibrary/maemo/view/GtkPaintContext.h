/*
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

#ifndef __GTKPAINTCONTEXT_H__
#define __GTKPAINTCONTEXT_H__

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <pango/pango.h>

#include <abstract/ZLPaintContext.h>

class GtkPaintContext : public ZLPaintContext {

public:
	GtkPaintContext();
	~GtkPaintContext();

	GdkPixmap *pixmap() { return myPixmap; }
	void updatePixmap(GtkWidget *area, int w, int h);

	int width() const;
	int height() const;

	void clear(ZLColor color);

	void fillFamiliesList(std::vector<std::string> &families) const;
	const std::string realFontFamilyName(std::string &fontFamily) const;

	void setFont(const std::string &family, int size, bool bold, bool italic);
	void setColor(ZLColor color, LineStyle style = SOLID_LINE);
	void setFillColor(ZLColor color, FillStyle style = SOLID_FILL);

	int stringWidth(const char *str, int len) const;
	int spaceWidth() const;
	int stringHeight() const;
	void drawString(int x, int y, const char *str, int len);

	void drawImage(int x, int y, const ZLImageData &image);

	void drawLine(int x0, int y0, int x1, int y1);
	void fillRectangle(int x0, int y0, int x1, int y1);
	void drawFilledCircle(int x, int y, int r);

private:
	GdkPixmap *myPixmap;
	int myWidth, myHeight;

  PangoContext *myContext;

  PangoFontDescription *myFontDescription;
	mutable PangoAnalysis myAnalysis;
	PangoGlyphString *myString;

	GdkGC *myTextGC;
	GdkGC *myFillGC;
	GdkGC *myBackGC;

	std::vector<std::string> myFontFamilies;

	mutable int myStringHeight;
	mutable int mySpaceWidth;
};

#endif /* __GTKPAINTCONTEXT_H__ */
