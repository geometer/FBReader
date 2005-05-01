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

#include "GtkPaintContext.h"
#include "GtkWord.h"

#include "../common/model/Image.h"

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
		g_object_unref(myContext);
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

static GdkColor BG_COLOR;
static GdkColor TX_COLOR;
static GdkColor FL_COLOR;

void GtkPaintContext::updatePixmap(GtkWidget *area) {
	int newWidth = area->allocation.width;
	int newHeight = area->allocation.height;
	if ((myPixmap != 0) && ((myWidth != newWidth) || (myHeight != newHeight))) {
		gdk_pixmap_unref(myPixmap);
		myPixmap = 0;
	}
	if (myPixmap == 0) {
		myWidth = newWidth;
		myHeight = newHeight;
		myPixmap = gdk_pixmap_new(area->window, myWidth, myHeight, gdk_drawable_get_depth(area->window));
	}

	if (myTextGC == 0) {
		GdkColormap *colormap = gdk_colormap_get_system();

		myTextGC = gdk_gc_new(myPixmap);
		TX_COLOR.red = 0x0000; TX_COLOR.green = 0x0000; TX_COLOR.blue = 0x0000;
		gdk_colormap_alloc_color(colormap, &TX_COLOR, false, false);
		gdk_gc_set_foreground(myTextGC, &TX_COLOR);
		BG_COLOR.red = 0xffff; BG_COLOR.green = 0xffff; BG_COLOR.blue = 0xffff;
		gdk_colormap_alloc_color(colormap, &BG_COLOR, false, false);
		gdk_gc_set_background(myTextGC, &BG_COLOR);

		myFillGC = gdk_gc_new(myPixmap);
		FL_COLOR.red = 0xd000; FL_COLOR.green = 0xd000; FL_COLOR.blue = 0xffff;
		gdk_colormap_alloc_color(colormap, &FL_COLOR, false, false);
		gdk_gc_set_foreground(myFillGC, &FL_COLOR);
	}

	if (myContext == 0) {
		myContext = gtk_widget_get_pango_context(area);

		myFontFamilies.clear();

		PangoFontFamily **pangoFamilies;
		int nFamilies;
		pango_context_list_families (myContext, &pangoFamilies, &nFamilies);
		for (int i = 0; i < nFamilies; i++) {
			myFontFamilies.push_back(pango_font_family_get_name(pangoFamilies[i]));
		}
		// TODO(?): sort list
		g_free(pangoFamilies);
	}

//	gdk_draw_rectangle(myPixmap, myTextGC, true, 0, 0, myWidth, myHeight);
	gdk_draw_rectangle(myPixmap, myFillGC, true, 0, 0, myWidth, myHeight);
}

void GtkPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	families = myFontFamilies;
}

const std::string GtkPaintContext::realFontFamilyName(std::string &fontFamily) const {
	/*
	QFont font;
	font.setFamily(fontFamily.c_str());
	return font.family().ascii();
	*/
	return fontFamily;
}

void GtkPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	if (myFont != 0) {
		pango_font_description_free(myFont);
	}

	myFont = pango_font_description_new();

	pango_font_description_set_family(myFont, family.c_str());
	pango_font_description_set_size(myFont, size * PANGO_SCALE);
	pango_font_description_set_weight(myFont, bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
	pango_font_description_set_style(myFont, italic ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL);
}

void GtkPaintContext::setColor(ZLColor color) {
#if 1
	GdkColormap *colormap = gdk_colormap_get_system();
	GdkColor gcColor;

	gcColor.red = color.Red * 65535 / 255;
	gcColor.blue = color.Blue * 65535 / 255;
	gcColor.green = color.Green * 65535 / 255;

	gdk_colormap_alloc_color(colormap, &gcColor, false, false);
	gdk_gc_set_foreground(myTextGC, &gcColor);
#endif
	//myPainter->setPen(QColor(color.Red, color.Green, color.Blue));
}

void GtkPaintContext::setFillColor(ZLColor color) {
	//myPainter->setBrush(QColor(color.Red, color.Green, color.Blue));
}

static void pango_text_size (PangoContext *context, PangoFontDescription *fdesc, const std::string& text, int *width, int *height) {
	PangoLayout *layout = pango_layout_new(context);

	pango_layout_set_text(layout, text.data(), text.size());
	pango_layout_set_font_description(layout, fdesc);

	pango_layout_get_size(layout, width, height);

//	std::cout << "pango_layout_get_width: " << text << ": " << width << ", " << height << std::endl;

	g_object_unref(layout);
}

static int pango_text_width (PangoContext *context, PangoFontDescription *fdesc, const std::string& text) {
	int width, height;

	pango_text_size (context, fdesc, text, &width, &height);

	return width / PANGO_SCALE;
}

static int pango_text_height (PangoContext *context, PangoFontDescription *fdesc, const std::string& text) {
	int width, height;

	pango_text_size (context, fdesc, text, &width, &height);

	return height / PANGO_SCALE;
}

int GtkPaintContext::wordWidth(const Word &word, int start, int length, bool addHyphenationSign) const {
	/* FIXME: we should somehow check if context is good. what to return if it's not good? */

	std::string tempo;

	if ((start == 0) && (length == -1)) {
		tempo = ((GtkWord&)word).utf8String();
	} else {
		tempo = ((GtkWord&)word).mid(start, length);
		if (addHyphenationSign) {
			tempo.append("-");
		}
	}

	return pango_text_width(myContext, myFont, tempo);
}

int GtkPaintContext::spaceWidth() const {
	return pango_text_width(myContext, myFont, " ");
}

int GtkPaintContext::wordHeight() const {
	return pango_text_height(myContext, myFont, "X");
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

void GtkPaintContext::drawWord(int x, int y, const Word &word, int start, int length, bool addHyphenationSign) {
	x += leftMargin().value();
	y += topMargin().value() - this->wordHeight();

	std::string what;

	if ((start == 0) && (length == -1)) {
		what = ((GtkWord&)word).utf8String();
	} else {
		what = ((GtkWord&)word).mid(start, length);
		if (addHyphenationSign) {
			what.append("-");
		}
	}

	PangoLayout *layout = pango_layout_new(myContext);

	pango_layout_set_text(layout, what.data(), what.size());
	pango_layout_set_font_description(layout, myFont);
	gdk_draw_layout (myPixmap, myTextGC, x, y, layout);

	g_object_unref(layout);
#if 0
	int len = word.length();
	for (int i = 0; i < len; i++) {
		GdkWChar wchar = word.charAt(i);
		gdk_draw_text_wc(myPixmap, myFont, myTextGC, x, y, &wchar, 1);
		x += gdk_char_width_wc(myFont, wchar);
	}
#endif
	/*
	if ((start == 0) && (length == -1)) {
		drawQString(x, y, ((QWord&)word).myValue, word.mark());
	} else {
		QString subword = ((QWord&)word).myValue.mid(start, length);
		if (addHyphenationSign) {
			subword.append("-");
		}
		drawQString(x, y, subword, word.mark(), start);
	}
	*/
}

/*
QImage &GtkPaintContext::qImage(const Image &image) const {
	QImage *imageRef = myImageCache[&image];
	if (imageRef == NULL) {
		imageRef = new QImage();
		imageRef->loadFromData(image.data(), image.datalen());
		myImageCache[&image] = imageRef;
	}
	return *imageRef;
}
*/

int GtkPaintContext::imageWidth(const Image &image) const {
	return 100;
	/*
	int w = qImage(image).width();
	int maxW = width();
	return (w <= maxW) ? w : maxW;
	*/
}

int GtkPaintContext::imageHeight(const Image &image) const {
	//return qImage(image).height();
	return 100;
}

void GtkPaintContext::drawImage(int x, int y, const Image &image) {
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
	/*
	if (myPixmap != NULL) {
		ZLColor background = BackgroundColorOption.value();
		myPixmap->fill(QColor(background.Red, background.Green, background.Blue));
		mySelectedTextColor = SelectedTextColorOption.value();
	}
	*/
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
