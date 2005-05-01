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

#include <algorithm>

#include <abstract/ZLUnicodeUtil.h>

#include "GtkPaintContext.h"

#include "../common/model/Image.h"

static void fillGdkColor(GdkColor &gdkColor, const ZLColor &zlColor) {
	gdkColor.red = zlColor.Red * 257;
	gdkColor.green = zlColor.Green * 257;
	gdkColor.blue = zlColor.Blue * 257;
}

GtkPaintContext::GtkPaintContext() {
	myPixmap = 0;
	myWidth = 0;
	myHeight = 0;

	myFont = 0;

	myContext = 0;

	myTextGC = 0;
	myFillGC = 0;
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
		// TODO: ???
		//g_object_unref(myContext);
	}
}

void GtkPaintContext::removeCaches() {
	PaintContext::removeCaches();

	/*
	for (std::map<const Image*,QImage*>::iterator it = myImageCache.begin(); it != myImageCache.end(); it++) {
		delete it->second;
	}
	myImageCache.clear();
	*/
}

void GtkPaintContext::updatePixmap(GtkWidget *area) {
	int newWidth = area->allocation.width;
	int newHeight = area->allocation.height;
	if ((myPixmap != 0) && ((myWidth != newWidth) || (myHeight != newHeight))) {
		gdk_pixmap_unref(myPixmap);
		myPixmap = 0;
		if (myTextGC != 0) {
			gdk_gc_unref(myTextGC);
			gdk_gc_unref(myFillGC);
			myTextGC = 0;
			myFillGC = 0;
		}
	}

	if (myPixmap == 0) {
		myWidth = newWidth;
		myHeight = newHeight;
		myPixmap = gdk_pixmap_new(area->window, myWidth, myHeight, gdk_drawable_get_depth(area->window));
	}

	if (myTextGC == 0) {
		myTextGC = gdk_gc_new(myPixmap);
		myFillGC = gdk_gc_new(myPixmap);
	}

	if (myContext == 0) {
		myContext = gtk_widget_get_pango_context(area);
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
}

void GtkPaintContext::setColor(ZLColor color) {
	GdkColormap *colormap = gdk_colormap_get_system();
	// TODO: check, if using of local variable is correct in this situation
	GdkColor gdkColor;
	fillGdkColor(gdkColor, color);
	if (gdk_colormap_alloc_color(colormap, &gdkColor, false, false)) {
		gdk_gc_set_foreground(myTextGC, &gdkColor);
	}
}

void GtkPaintContext::setFillColor(ZLColor color) {
	GdkColormap *colormap = gdk_colormap_get_system();
	GdkColor gdkColor;
	fillGdkColor(gdkColor, color);
	if (gdk_colormap_alloc_color(colormap, &gdkColor, false, false)) {
		gdk_gc_set_foreground(myFillGC, &gdkColor);
	}
}

static void pango_text_size (PangoContext *context, PangoFontDescription *fdesc, const char *text, int len, int *width, int *height) {
	/* FIXME: we should somehow check if context is good. what to return if it's not good? */

	PangoLayout *layout = pango_layout_new(context);

	pango_layout_set_text(layout, text, len);
	pango_layout_set_font_description(layout, fdesc);

	pango_layout_get_pixel_size(layout, width, height);

//	std::cout << "pango_layout_get_width: " << text << ": " << width << ", " << height << std::endl;

	g_object_unref(layout);
}

static int pango_text_width (PangoContext *context, PangoFontDescription *fdesc, const char *text, int len) {
	int width, height;

	pango_text_size (context, fdesc, text, len, &width, &height);

	return width;
}

static int pango_text_height (PangoContext *context, PangoFontDescription *fdesc, const char *text, int len) {
	int width, height;

	pango_text_size (context, fdesc, text, len, &width, &height);

	return height;
}

int GtkPaintContext::wordWidth(const Word &word, int start, int length, bool addHyphenationSign) const {
	const std::string &str = word.utf8String();
	int startPos = ZLUnicodeUtil::length(str, start);
	int endPos = (length != -1) ? ZLUnicodeUtil::length(str, start + length) : str.length();
	int len = endPos - startPos;

	if (!addHyphenationSign) {
		return pango_text_width(myContext, myFont, str.data() + startPos, len);
	} else {
		std::string sstr = str.substr(startPos, len) + '-';
		return pango_text_width(myContext, myFont, sstr.data(), sstr.length());
	}
}

int GtkPaintContext::spaceWidth() const {
	return pango_text_width(myContext, myFont, " ", 1);
}

int GtkPaintContext::wordHeight() const {
	return pango_text_height(myContext, myFont, "X", 1);
}

/*
void GtkPaintContext::drawString(int x, int y, const std::string &str, const Word::WordMark *mark, int shift) {
	if (mark == 0) {
		myPainter->drawText(x, y, str);
	} else {
		int currentLetter = 0;
		for (; (mark != 0) && (currentLetter < (int)str.length()); mark = mark->next()) {
			int markStart = mark->start() - shift;
			int markLen = mark->length();

			if (markStart < currentLetter) {
				markLen += markStart - currentLetter;
				markStart = currentLetter;
			}

			if (markLen <= 0) {
				continue;
			}

			if (markStart > currentLetter) {
				QString beforeMark = str.mid(currentLetter, markStart - currentLetter);
				myPainter->drawText(x, y, beforeMark);
				x += myPainter->fontMetrics().width(beforeMark);
			}
			QString insideMark = str.mid(markStart, markLen);
			setColor(mySelectedTextColor);
			myPainter->drawText(x, y, insideMark);
			setColor(myTextColor);
			x += myPainter->fontMetrics().width(insideMark);
			currentLetter = markStart + markLen;
		}

		if (currentLetter < (int)str.length()) {
			QString afterMark = str.mid(currentLetter);
			myPainter->drawText(x, y, afterMark);
		}
	}
}
*/

void GtkPaintContext::drawString(int x, int y, const std::string &str, int from, int len) {
	PangoLayout *layout = pango_layout_new(myContext);
	pango_layout_set_text(layout, str.data() + from, len);
	pango_layout_set_font_description(layout, myFont);
	gdk_draw_layout (myPixmap, myTextGC, x, y, layout);
	g_object_unref(layout);
}

void GtkPaintContext::drawWord(int x, int y, const Word &word, int start, int length, bool addHyphenationSign) {
	x += leftMargin().value();
	y += topMargin().value() - this->wordHeight();

	const std::string &str = word.utf8String();
	int startPos = ZLUnicodeUtil::length(str, start);
	int endPos = (length != -1) ? ZLUnicodeUtil::length(str, start + length) : str.length();
	int len = endPos - startPos;

	if (!addHyphenationSign) {
		drawString(x, y, str, startPos, len);
	} else {
		drawString(x, y, str.substr(startPos, len) + '-', 0, len + 1);
	}
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
		GdkColormap *colormap = gdk_colormap_get_system();
		GdkColor bgColor;
		fillGdkColor(bgColor, BackgroundColorOption.value());
		if (gdk_colormap_alloc_color(colormap, &bgColor, false, false)) {
			GdkGC *bgGC = gdk_gc_new(myPixmap);
			gdk_gc_set_foreground(bgGC, &bgColor);
			gdk_draw_rectangle(myPixmap, bgGC, true, 0, 0, myWidth, myHeight);
			gdk_gc_unref(bgGC);
		}
		// TODO: setup selected text color
		/*
		fillGdkColor(selectedColor, SelectedTextColorOption.value());
		*/
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
