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


#include <abstract/ZLUnicodeUtil.h>

#include "GtkPaintContext.h"

#include "../common/model/Image.h"

static void setColor(GdkGC *gc, const ZLColor &zlColor) {
	if (gc != 0) {
		// TODO: check, if using of local variable is correct in this situation
		GdkColor gdkColor;
		gdkColor.red = zlColor.Red * 257;
		gdkColor.green = zlColor.Green * 257;
		gdkColor.blue = zlColor.Blue * 257;
		GdkColormap *colormap = gdk_colormap_get_system();
		if (gdk_colormap_alloc_color(colormap, &gdkColor, false, false)) {
			gdk_gc_set_foreground(gc, &gdkColor);
		}
	}
}

GtkPaintContext::GtkPaintContext() {
	myPixmap = 0;
	myWidth = 0;
	myHeight = 0;

	myFont = 0;

	myContext = 0;
	myLayout = 0;

	myTextGC = 0;
	myFillGC = 0;
	myBackGC = 0;
}

GtkPaintContext::~GtkPaintContext() {
	if (myPixmap != 0) {
		gdk_pixmap_unref(myPixmap);
	}
	if (myTextGC) {
		gdk_gc_unref(myTextGC);
		gdk_gc_unref(myFillGC);
	}

	if (myFont != 0) {
		pango_font_description_free(myFont);
	}

	if (myContext != 0) {
		g_object_unref(myLayout);
		g_object_unref(myContext);
	}
}

void GtkPaintContext::removeCaches() {
	PaintContext::removeCaches();

	for (std::map<const Image*, GdkPixbuf *>::iterator it = myImageCache.begin(); it != myImageCache.end(); it++) {
		g_object_unref(it->second);
	}

	myImageCache.clear();
}

void GtkPaintContext::updatePixmap(GtkWidget *area, int w, int h) {
	if ((myPixmap != 0) && ((myWidth != w) || (myHeight != h))) {
		gdk_pixmap_unref(myPixmap);
		myPixmap = 0;
		if (myTextGC != 0) {
			gdk_gc_unref(myTextGC);
			gdk_gc_unref(myFillGC);
			gdk_gc_unref(myBackGC);
			myTextGC = 0;
			myFillGC = 0;
			myBackGC = 0;
		}
	}

	if (myPixmap == 0) {
		myWidth = w;
		myHeight = h;
		myPixmap = gdk_pixmap_new(area->window, myWidth, myHeight, gdk_drawable_get_depth(area->window));
	}

	if (myTextGC == 0) {
		myTextGC = gdk_gc_new(myPixmap);
		myFillGC = gdk_gc_new(myPixmap);
		myBackGC = gdk_gc_new(myPixmap);
	}

	if (myContext == 0) {
		myContext = gtk_widget_get_pango_context(area);
		myLayout = pango_layout_new(myContext);
	}
}

void GtkPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	if (myContext != 0) {
		PangoFontFamily **pangoFamilies;
		int nFamilies;
		pango_context_list_families(myContext, &pangoFamilies, &nFamilies);
		for (int i = 0; i < nFamilies; i++) {
			families.push_back(pango_font_family_get_name(pangoFamilies[i]));
		}
		std::sort(families.begin(), families.end());
		g_free(pangoFamilies);
	}
}

const std::string GtkPaintContext::realFontFamilyName(std::string &fontFamily) const {
	// TODO: implement
	return fontFamily;
}

void GtkPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	if (myFont == 0) {
		myFont = pango_font_description_new();
	}

	pango_font_description_set_family(myFont, family.c_str());
	pango_font_description_set_size(myFont, size * PANGO_SCALE);
	pango_font_description_set_weight(myFont, bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
	pango_font_description_set_style(myFont, italic ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL);
	pango_layout_set_font_description(myLayout, myFont);
}

void GtkPaintContext::setColor(ZLColor color) {
	::setColor(myTextGC, color);
}

void GtkPaintContext::setFillColor(ZLColor color) {
	::setColor(myFillGC, color);
}

int GtkPaintContext::stringWidth(const std::string &str, int from, int len) const {
	if (myLayout == 0) {
		return 0;
	}
	int width, height;
	pango_layout_set_text(myLayout, str.data() + from, len);
	pango_layout_get_pixel_size(myLayout, &width, &height);
	return width;
}

int GtkPaintContext::stringHeight() const {
	if (myLayout == 0) {
		return 0;
	}
	int width, height;
	pango_layout_set_text(myLayout, "X", 1);
	pango_layout_get_pixel_size(myLayout, &width, &height);
	return height;
}

void GtkPaintContext::drawString(int x, int y, const std::string &str, int from, int len) {
	pango_layout_set_text(myLayout, str.data() + from, len);
	x += leftMargin().value();
	y += topMargin().value() - pango_font_description_get_size(myFont) / PANGO_SCALE;
	gdk_draw_layout(myPixmap, myTextGC, x, y, myLayout);
}

GdkPixbuf *GtkPaintContext::gtkImage(const Image &image) const {
	GdkPixbuf *imageRef = myImageCache[&image];
	if (imageRef == NULL) {
		GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
		GError *error = NULL;

		// TODO: properly process return value and error code
		gdk_pixbuf_loader_write(loader, image.data(),image.datalen(), &error);
		gdk_pixbuf_loader_close(loader, &error);

		if (error != NULL) {
			g_error_free(error);
		}

		imageRef = gdk_pixbuf_loader_get_pixbuf(loader);
		g_object_ref(imageRef);

		g_object_unref(loader);

		myImageCache[&image] = imageRef;
	}

	return imageRef;
}

int GtkPaintContext::imageWidth(const Image &image) const {
	GdkPixbuf *imageRef = gtkImage(image);
	return gdk_pixbuf_get_width(imageRef);
}

int GtkPaintContext::imageHeight(const Image &image) const {
	GdkPixbuf *imageRef = gtkImage(image);
	return gdk_pixbuf_get_height(imageRef);
}

void GtkPaintContext::drawImage(int x, int y, const Image &image) {
	// TODO: should we optimize it all? we do two lookups in our cache
	// for gtk+ v2.2+ gdk_draw_pixbuf (myPixmap, NULL, gtkImage(image), 0, 0, x + leftMargin().value(), y + topMargin().value() - imageHeight(image), -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
	gdk_pixbuf_render_to_drawable (gtkImage(image), myPixmap, NULL, 0, 0, x + leftMargin().value(), y + topMargin().value() - imageHeight(image), -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
	//
	// COMMENTS:
	// NULL			-- we have no clipping (do we need it?)
	// 0, 0			-- offset in the image
	// -1, -1		-- use the whole
	// GDK_RGB_DITHER_NONE -- no dithering, hopefully, (0, 0) after it does not harm

	//myPainter->drawImage(x + leftMargin().value(), y + topMargin().value() - imageHeight(image), qImage(image));
}

void GtkPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	gdk_draw_line(myPixmap, myTextGC,
								x0 + leftMargin().value(), y0 + topMargin().value(),
								x1 + leftMargin().value(), y1 + topMargin().value());
}

void GtkPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	if (x1 < x0) {
		int tmp = x1;
		x1 = x0;
		x0 = tmp;
	}
	if (y1 < y0) {
		int tmp = y1;
		y1 = y0;
		y0 = tmp;
	}
	gdk_draw_rectangle(myPixmap, myFillGC, true,
										 x0 + leftMargin().value(), y0 + topMargin().value(),
										 x1 - x0 + 1, y1 - y0 + 1);
}

void GtkPaintContext::clear() {
	if (myPixmap != NULL) {
		::setColor(myBackGC, BackgroundColorOption.value());
		gdk_draw_rectangle(myPixmap, myBackGC, true, 0, 0, myWidth, myHeight);
	}
}

int GtkPaintContext::width() const {
	if (myPixmap == NULL) {
		return 0;
	}
	return myWidth - leftMargin().value() - rightMargin().value();
}

int GtkPaintContext::height() const {
	if (myPixmap == NULL) {
		return 0;
	}
	return myHeight - bottomMargin().value() - topMargin().value();
}

// vim:ts=2:sw=2:noet
