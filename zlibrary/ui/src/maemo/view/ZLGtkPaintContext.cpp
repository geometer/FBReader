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

#include <algorithm>

#include <ZLUnicodeUtil.h>
#include <ZLImage.h>

#include "ZLGtkPaintContext.h"
#include "../../gtk/image/ZLGtkImageManager.h"

static bool setColor(GdkColor &gdkColor, const ZLColor &zlColor) {
	gdkColor.red = zlColor.Red * (65535 / 255);
	gdkColor.green = zlColor.Green * (65535 / 255);
	gdkColor.blue = zlColor.Blue * (65535 / 255);
	GdkColormap *colormap = gdk_colormap_get_system();
	return gdk_colormap_alloc_color(colormap, &gdkColor, false, false);
}

static void setColor(GdkGC *gc, const ZLColor &zlColor) {
	if (gc != 0) {
		GdkColor gdkColor;
		if (setColor(gdkColor, zlColor)) {
			gdk_gc_set_foreground(gc, &gdkColor);
		}
	}
}

ZLGtkPaintContext::ZLGtkPaintContext() {
	myPixmap = 0;
	myTilePixmap = 0;
	myWidth = 0;
	myHeight = 0;

	myContext = 0;

	myFontDescription = 0;
	myAnalysis.lang_engine = 0;
	myAnalysis.level = 0;
	myAnalysis.language = 0;
	myAnalysis.extra_attrs = 0;
	myString = pango_glyph_string_new();

	myTextGC = 0;
	myFillGC = 0;
	myBackGC = 0;

	myStringHeight = -1;
	mySpaceWidth = -1;
	myDescent = 0;
}

ZLGtkPaintContext::~ZLGtkPaintContext() {
	if (myPixmap != 0) {
		gdk_pixmap_unref(myPixmap);
	}
	if (myTextGC) {
		gdk_gc_unref(myTextGC);
		gdk_gc_unref(myFillGC);
	}

	pango_glyph_string_free(myString);
	
	if (myFontDescription != 0) {
		pango_font_description_free(myFontDescription);
	}

	if (myContext != 0) {
		g_object_unref(myContext);
	}
}

void ZLGtkPaintContext::updatePixmap(GtkWidget *area, int w, int h) {
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
		if (myFontDescription != 0) {
			myAnalysis.font = pango_context_load_font(myContext, myFontDescription);
			myAnalysis.shape_engine = pango_font_find_shaper(myAnalysis.font, 0, 0);
			PangoFontMetrics *metrics = pango_font_get_metrics(myAnalysis.font, myAnalysis.language);
			myDescent = pango_font_metrics_get_descent(metrics) / PANGO_SCALE;
		}
	}
}

void ZLGtkPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	if (myContext != 0) {
		PangoFontFamily **pangoFamilies;
		int nFamilies;
		pango_context_list_families(myContext, &pangoFamilies, &nFamilies);
		for (int i = 0; i < nFamilies; ++i) {
			families.push_back(pango_font_family_get_name(pangoFamilies[i]));
		}
		std::sort(families.begin(), families.end());
		g_free(pangoFamilies);
	}
}

const std::string ZLGtkPaintContext::realFontFamilyName(std::string &fontFamily) const {
	if (myContext == 0) {
		return fontFamily;
	}
	if ((fontFamily == "Sans") || (fontFamily == "Serif") || (fontFamily == "Monospace")) {
		return fontFamily;
	}
	PangoFontDescription *description = pango_font_description_new();
	pango_font_description_set_family(description, fontFamily.c_str());
	pango_font_description_set_size(description, 12);
	PangoFont *font = pango_context_load_font(myContext, description);
	pango_font_description_free(description);
	description = pango_font_describe(font);
	std::string realFamily = pango_font_description_get_family(description);
	pango_font_description_free(description);
	return realFamily;
}

void ZLGtkPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	bool fontChanged = false;

	if (myFontDescription == 0) {
		myFontDescription = pango_font_description_new();
		fontChanged = true;
	}

	const char *oldFamily = pango_font_description_get_family(myFontDescription);
	if ((oldFamily == 0) || (family != oldFamily)) {
		pango_font_description_set_family(myFontDescription, family.c_str());
		fontChanged = true;
	}

	int newSize = size * PANGO_SCALE;
	if (pango_font_description_get_size(myFontDescription) != newSize) {
		pango_font_description_set_size(myFontDescription, newSize);
		fontChanged = true;
	}

	PangoWeight newWeight = bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL;
	if (pango_font_description_get_weight(myFontDescription) != newWeight) {
		pango_font_description_set_weight(myFontDescription, newWeight);
		fontChanged = true;
	}

	PangoStyle newStyle = italic ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL;
	if (pango_font_description_get_style(myFontDescription) != newStyle) {
		pango_font_description_set_style(myFontDescription, newStyle);
		fontChanged = true;
	}

	if (fontChanged) {
		if (myContext != 0) {
			myAnalysis.font = pango_context_load_font(myContext, myFontDescription);
			myAnalysis.shape_engine = pango_font_find_shaper(myAnalysis.font, 0, 0);
			PangoFontMetrics *metrics = pango_font_get_metrics(myAnalysis.font, myAnalysis.language);
			myDescent = pango_font_metrics_get_descent(metrics) / PANGO_SCALE;
		}
		myStringHeight = -1;
		mySpaceWidth = -1;
	}
}

void ZLGtkPaintContext::setColor(ZLColor color, LineStyle style) {
	::setColor(myTextGC, color);
	gdk_gc_set_line_attributes(myTextGC, 0, (style == SOLID_LINE) ? GDK_LINE_SOLID : GDK_LINE_ON_OFF_DASH, (GdkCapStyle)1, (GdkJoinStyle)1);
}

void ZLGtkPaintContext::setFillColor(ZLColor color, FillStyle style) {
	if (style == SOLID_FILL) {
		::setColor(myFillGC, color);
		gdk_gc_set_fill(myFillGC, GDK_SOLID);
	} else {
		gdk_gc_set_fill(myFillGC, GDK_TILED);
		if (myPixmap != 0) {
			if (myTilePixmap != 0) {
				gdk_pixmap_unref(myTilePixmap);
			}
			static GdkColor fgColor;
			::setColor(fgColor, color);
			static GdkColor bgColor;
			::setColor(bgColor, myBackColor);
			static char data[] = { 0x0C, 0x0C, 0x03, 0x03 };
			myTilePixmap = gdk_pixmap_create_from_data(
				myPixmap, data, 4, 4, gdk_drawable_get_depth(myPixmap), &fgColor, &bgColor
			);
			gdk_gc_set_tile(myFillGC, myTilePixmap);
		}
	}
}

int ZLGtkPaintContext::stringWidth(const char *str, int len) const {
	if (myContext == 0) {
		return 0;
	}

	if (!g_utf8_validate(str, len, 0)) {
		return 0;
	}

	pango_shape(str, len, &myAnalysis, myString);
	PangoRectangle logicalRectangle;
	pango_glyph_string_extents(myString, myAnalysis.font, 0, &logicalRectangle);
	return (logicalRectangle.width + PANGO_SCALE / 2) / PANGO_SCALE;
}

int ZLGtkPaintContext::spaceWidth() const {
	if (mySpaceWidth == -1) {
		mySpaceWidth = stringWidth(" ", 1);
	}
	return mySpaceWidth;
}

int ZLGtkPaintContext::stringHeight() const {
	if (myFontDescription == 0) {
		return 0;
	}
	if (myStringHeight == -1) {
		myStringHeight = pango_font_description_get_size(myFontDescription) / PANGO_SCALE + 2;
	}
	return myStringHeight;
}

int ZLGtkPaintContext::descent() const {
	return myDescent;
}

void ZLGtkPaintContext::drawString(int x, int y, const char *str, int len) {
	if (!g_utf8_validate(str, len, 0)) {
		return;
	}

	pango_shape(str, len, &myAnalysis, myString);
	gdk_draw_glyphs(myPixmap, myTextGC, myAnalysis.font, x, y, myString);
}

void ZLGtkPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	GdkPixbuf *imageRef = ((ZLGtkImageData&)image).pixbuf();
	if (imageRef != 0) {
		gdk_pixbuf_render_to_drawable(
			imageRef, myPixmap,
			0, 0, 0,
			x, y - gdk_pixbuf_get_height(imageRef),
			-1, -1, GDK_RGB_DITHER_NONE, 0, 0
		);
	}
	// for gtk+ v2.2
	// 		gdk_draw_pixbuf(
	// 			myPixmap, 0, imageRef, 0, 0,
	// 			x, y - gdk_pixbuf_get_height(imageRef),
	// 			-1, -1, GDK_RGB_DITHER_NONE, 0, 0
	// 		);
	//
	// COMMENTS:
	// 0			-- we have no clipping (do we need it?)
	// 0, 0			-- offset in the image
	// -1, -1		-- use the whole
	// GDK_RGB_DITHER_NONE -- no dithering, hopefully, (0, 0) after it does not harm
}

void ZLGtkPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	gdk_draw_line(myPixmap, myTextGC, x0, y0, x1, y1);
}

void ZLGtkPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
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
										 x0, y0,
										 x1 - x0 + 1, y1 - y0 + 1);
}

void ZLGtkPaintContext::drawFilledCircle(int x, int y, int r) {
	gdk_draw_arc(myPixmap, myFillGC, true, x - r, y - r, 2 * r + 1, 2 * r + 1, 0, 360 * 64);
	gdk_draw_arc(myPixmap, myTextGC, false, x - r, y - r, 2 * r + 1, 2 * r + 1, 0, 360 * 64);
}

void ZLGtkPaintContext::clear(ZLColor color) {
	myBackColor = color;
	if (myPixmap != 0) {
		::setColor(myBackGC, color);
		gdk_draw_rectangle(myPixmap, myBackGC, true, 0, 0, myWidth, myHeight);
	}
}

int ZLGtkPaintContext::width() const {
	if (myPixmap == 0) {
		return 0;
	}
	return myWidth;
}

int ZLGtkPaintContext::height() const {
	if (myPixmap == 0) {
		return 0;
	}
	return myHeight;
}
