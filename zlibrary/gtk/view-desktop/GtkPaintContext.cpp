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

#include <ZLUnicodeUtil.h>
#include <ZLImage.h>

#include "GtkPaintContext.h"
#include "../image/GtkImageManager.h"

#include "gdk-pixbuf-hack.h"

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

GtkPaintContext::GtkPaintContext() {
	myPixmap = 0;
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

	myTilePixmap = 0;

	myStringHeight = -1;
	mySpaceWidth = -1;

	myWordPixmap = 0;

	myAngle = ZLViewWidget::DEGREES0;
}

GtkPaintContext::~GtkPaintContext() {
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

	if (myWordPixmap != 0) {
		gdk_pixbuf_unref(myWordPixbuf);
		gdk_pixbuf_unref(myRotatedWordPixbuf);
		gdk_image_unref(myWordImage);
		gdk_pixmap_unref(myWordPixmap);
	}
}

inline void GtkPaintContext::rotatePoint(int &x, int &y) const {
	switch (myAngle) {
		default:
			break;
		case ZLViewWidget::DEGREES90:
		{
			int tmp = x;
			x = y;
			y = myHeight - tmp - 1;
			break;
		}
		case ZLViewWidget::DEGREES180:
			x = myWidth - x - 1;
			y = myHeight - y - 1;
			break;
		case ZLViewWidget::DEGREES270:
		{
			int tmp = x;
			x = myWidth - y - 1;
			y = tmp;
			break;
		}
	}
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
		if (myFontDescription != 0) {
			myAnalysis.font = pango_context_load_font(myContext, myFontDescription);
			myAnalysis.shape_engine = pango_font_find_shaper(myAnalysis.font, 0, 0);
		}
	}
}

void GtkPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
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

const std::string GtkPaintContext::realFontFamilyName(std::string &fontFamily) const {
	if (myContext == 0) {
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

void GtkPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
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
		}
		myStringHeight = -1;
		mySpaceWidth = -1;
	}
}

void GtkPaintContext::setColor(ZLColor color, LineStyle style) {
	::setColor(myTextGC, color);
	gdk_gc_set_line_attributes(myTextGC, 0, (style == SOLID_LINE) ? GDK_LINE_SOLID : GDK_LINE_ON_OFF_DASH, (GdkCapStyle)1, (GdkJoinStyle)1);
}

void GtkPaintContext::setFillColor(ZLColor color, FillStyle style) {
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

int GtkPaintContext::stringWidth(const char *str, int len) const {
	if (myContext == 0) {
		return 0;
	}

	if (!g_utf8_validate(str, len, 0)) {
		return 0;
	}

	pango_shape(str, len, &myAnalysis, myString);
	PangoRectangle logicalRectangle;
	pango_glyph_string_extents(myString, myAnalysis.font, 0, &logicalRectangle);
	std::string XXX;
	XXX.append(str, len);
	return (logicalRectangle.width + PANGO_SCALE / 2) / PANGO_SCALE;
}

int GtkPaintContext::spaceWidth() const {
	if (mySpaceWidth == -1) {
		mySpaceWidth = stringWidth(" ", 1);
	}
	return mySpaceWidth;
}

int GtkPaintContext::stringHeight() const {
	if (myFontDescription == 0) {
		return 0;
	}
	if (myStringHeight == -1) {
		myStringHeight = pango_font_description_get_size(myFontDescription) / PANGO_SCALE + 2;
	}
	return myStringHeight;
}

void GtkPaintContext::drawString(int x, int y, const char *str, int len) {
	if (!g_utf8_validate(str, len, 0)) {
		return;
	}

	x += leftMargin();
	y += topMargin();

	rotatePoint(x, y);
	
	pango_shape(str, len, &myAnalysis, myString);
	switch (myAngle) {
		default:
			gdk_draw_glyphs(myPixmap, myTextGC, myAnalysis.font, x, y, myString);
			break;
		case ZLViewWidget::DEGREES180:
		{
			PangoRectangle logicalRectangle;
			pango_glyph_string_extents(myString, myAnalysis.font, 0, &logicalRectangle);
			const int w = (logicalRectangle.width + PANGO_SCALE / 2) / PANGO_SCALE;
			const int h = (logicalRectangle.height + PANGO_SCALE / 2) / PANGO_SCALE;
			const int ascent = (pango_font_metrics_get_ascent(pango_font_get_metrics(myAnalysis.font, 0)) + PANGO_SCALE / 2) / PANGO_SCALE;

			if (myWordPixmap != 0) {
				if ((gdk_pixbuf_get_width(myWordPixbuf) < w) ||
						(gdk_pixbuf_get_width(myRotatedWordPixbuf) < w) ||
						(gdk_pixbuf_get_height(myWordPixbuf) < h) ||
						(gdk_pixbuf_get_height(myRotatedWordPixbuf) < h)) {
					gdk_pixbuf_unref(myWordPixbuf);
					gdk_pixbuf_unref(myRotatedWordPixbuf);
					gdk_image_unref(myWordImage);
					gdk_pixmap_unref(myWordPixmap);
					myWordPixmap = 0;
				}
			}
			if (myWordPixmap == 0) {
				myWordPixmap = gdk_pixmap_new(myPixmap, w, h, gdk_drawable_get_depth(myPixmap));
				myWordImage = gdk_image_new(GDK_IMAGE_FASTEST, gdk_drawable_get_visual(myPixmap), w, h);
				myWordPixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, w, h);
				myRotatedWordPixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, w, h);
			}
			
			gdk_draw_rectangle(myWordPixmap, myBackGC, true, 0, 0, w, h);
			gdk_draw_glyphs(myWordPixmap, myTextGC, myAnalysis.font, 0, ascent, myString);
			gdk_drawable_copy_to_image(myWordPixmap, myWordImage, 0, 0, 0, 0, w, h);
			gdk_pixbuf_get_from_image(myWordPixbuf, myWordImage, gdk_drawable_get_colormap(myWordPixmap), 0, 0, 0, 0, w, h);
			::rotate180(myRotatedWordPixbuf, myWordPixbuf, w, h);
			gdk_draw_pixbuf(myPixmap, myTextGC, myRotatedWordPixbuf, 0, 0, x - w, y, w, h, GDK_RGB_DITHER_NONE, 0, 0);
			break;
		}
		case ZLViewWidget::DEGREES90:
		case ZLViewWidget::DEGREES270:
		{
			PangoRectangle logicalRectangle;
			pango_glyph_string_extents(myString, myAnalysis.font, 0, &logicalRectangle);
			const int w = (logicalRectangle.width + PANGO_SCALE / 2) / PANGO_SCALE;
			const int h = (logicalRectangle.height + PANGO_SCALE / 2) / PANGO_SCALE;
			const int ascent = (pango_font_metrics_get_ascent(pango_font_get_metrics(myAnalysis.font, 0)) + PANGO_SCALE / 2) / PANGO_SCALE;

			if (myWordPixmap != 0) {
				if ((gdk_pixbuf_get_width(myWordPixbuf) < w) ||
						(gdk_pixbuf_get_width(myRotatedWordPixbuf) < h) ||
						(gdk_pixbuf_get_height(myWordPixbuf) < h) ||
						(gdk_pixbuf_get_height(myRotatedWordPixbuf) < w)) {
					gdk_pixbuf_unref(myWordPixbuf);
					gdk_pixbuf_unref(myRotatedWordPixbuf);
					gdk_image_unref(myWordImage);
					gdk_pixmap_unref(myWordPixmap);
					myWordPixmap = 0;
				}
			}
			if (myWordPixmap == 0) {
				myWordPixmap = gdk_pixmap_new(myPixmap, w, h, gdk_drawable_get_depth(myPixmap));
				myWordImage = gdk_image_new(GDK_IMAGE_FASTEST, gdk_drawable_get_visual(myPixmap), w, h);
				myWordPixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, w, h);
				myRotatedWordPixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, h, w);
			}
			
			gdk_draw_rectangle(myWordPixmap, myBackGC, true, 0, 0, w, h);
			gdk_draw_glyphs(myWordPixmap, myTextGC, myAnalysis.font, 0, ascent, myString);
			gdk_drawable_copy_to_image(myWordPixmap, myWordImage, 0, 0, 0, 0, w, h);
			gdk_pixbuf_get_from_image(myWordPixbuf, myWordImage, gdk_drawable_get_colormap(myWordPixmap), 0, 0, 0, 0, w, h);
			::rotate90(myRotatedWordPixbuf, myWordPixbuf, w, h, myAngle == ZLViewWidget::DEGREES90);
			if (myAngle == ZLViewWidget::DEGREES90) {
				x -= ascent;
				y -= w;
			} else {
				const int descent = (pango_font_metrics_get_descent(pango_font_get_metrics(myAnalysis.font, 0)) + PANGO_SCALE / 2) / PANGO_SCALE;
				x += h - ascent - descent;
			}
			gdk_draw_pixbuf(myPixmap, myTextGC, myRotatedWordPixbuf, 0, 0, x, y, h, w, GDK_RGB_DITHER_NONE, 0, 0);
			break;
		}
	}
}

void GtkPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	GdkPixbuf *imageRef = ((GtkImageData&)image).pixbuf();
	if (imageRef != 0) {
		x += leftMargin();
		y += topMargin();

		switch (myAngle) {
			default:
				gdk_pixbuf_render_to_drawable(
					imageRef, myPixmap,
					0, 0, 0,
					x, y - gdk_pixbuf_get_height(imageRef),
					-1, -1, GDK_RGB_DITHER_NONE, 0, 0
				);
				break;
			case ZLViewWidget::DEGREES180:
			{
				const int w = gdk_pixbuf_get_width(imageRef);
				const int h = gdk_pixbuf_get_height(imageRef);
				rotatePoint(x, y);
				GdkPixbuf *rotated = gdk_pixbuf_new(GDK_COLORSPACE_RGB, gdk_pixbuf_get_has_alpha(imageRef), 8, w, h);
				::rotate180(rotated, imageRef, w, h);
				gdk_pixbuf_render_to_drawable(
					rotated, myPixmap,
					0, 0, 0,
					x - w, y,
					w, h, GDK_RGB_DITHER_NONE, 0, 0
				);
				gdk_pixbuf_unref(rotated);
				break;
			}
			case ZLViewWidget::DEGREES90:
			case ZLViewWidget::DEGREES270:
			{
				const int w = gdk_pixbuf_get_width(imageRef);
				const int h = gdk_pixbuf_get_height(imageRef);
				rotatePoint(x, y);
				GdkPixbuf *rotated = gdk_pixbuf_new(GDK_COLORSPACE_RGB, gdk_pixbuf_get_has_alpha(imageRef), 8, h, w);
				::rotate90(rotated, imageRef, w, h, myAngle == ZLViewWidget::DEGREES90);
				if (myAngle == ZLViewWidget::DEGREES90) {
					x -= h;
					y -= w;
				}
				gdk_pixbuf_render_to_drawable(
					rotated, myPixmap,
					0, 0, 0,
					x, y,
					h, w, GDK_RGB_DITHER_NONE, 0, 0
				);
				gdk_pixbuf_unref(rotated);
				break;
			}
		}
	}
}

void GtkPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	x0 += leftMargin();
	x1 += leftMargin();
	y0 += topMargin();
	y1 += topMargin();

	rotatePoint(x0, y0);
	rotatePoint(x1, y1);

	gdk_draw_line(myPixmap, myTextGC, x0, y0, x1, y1);
}

void GtkPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	x0 += leftMargin();
	x1 += leftMargin();
	y0 += topMargin();
	y1 += topMargin();

	rotatePoint(x0, y0);
	rotatePoint(x1, y1);

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
	gdk_draw_rectangle(myPixmap, myFillGC, true, x0, y0, x1 - x0 + 1, y1 - y0 + 1);
}

void GtkPaintContext::drawFilledCircle(int x, int y, int r) {
	x += leftMargin();
	y += topMargin();
	rotatePoint(x, y);

	gdk_draw_arc(myPixmap, myFillGC, true, x - r, y - r, 2 * r + 1, 2 * r + 1, 0, 360 * 64);
	gdk_draw_arc(myPixmap, myTextGC, false, x - r, y - r, 2 * r + 1, 2 * r + 1, 0, 360 * 64);
}

void GtkPaintContext::clear(ZLColor color) {
	myBackColor = color;
	if (myPixmap != NULL) {
		::setColor(myBackGC, color);
		gdk_draw_rectangle(myPixmap, myBackGC, true, 0, 0, myWidth, myHeight);
	}
}

int GtkPaintContext::width() const {
	if (myPixmap == NULL) {
		return 0;
	}
	if ((myAngle == ZLViewWidget::DEGREES90) || (myAngle == ZLViewWidget::DEGREES270)) {
		return myHeight - leftMargin() - rightMargin();
	} else {
		return myWidth - leftMargin() - rightMargin();
	}
}

int GtkPaintContext::height() const {
	if (myPixmap == NULL) {
		return 0;
	}
	if ((myAngle == ZLViewWidget::DEGREES90) || (myAngle == ZLViewWidget::DEGREES270)) {
		return myWidth - bottomMargin() - topMargin();
	} else {
		return myHeight - bottomMargin() - topMargin();
	}
}

// vim:ts=2:sw=2:noet
