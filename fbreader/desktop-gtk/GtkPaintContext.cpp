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
//#include "QWord.h"

#include "../common/model/Image.h"

GtkPaintContext::GtkPaintContext() {
	myPixmap = 0;
	myWidth = 0;
	myHeight = 0;

	myFont = 0;

	myBackgroundGC = 0;
	myTextGC = 0;
	myFillGC = 0;
}

GtkPaintContext::~GtkPaintContext() {
	if (myPixmap != 0) {
		gdk_pixmap_unref(myPixmap);
	}
	if (myBackgroundGC) {
		gdk_gc_unref(myBackgroundGC);
		gdk_gc_unref(myTextGC);
		gdk_gc_unref(myFillGC);
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

	if (myBackgroundGC == 0) {
		GdkColormap *colormap = gdk_colormap_get_system();

		myBackgroundGC = gdk_gc_new(myPixmap);
		BG_COLOR.red = 0xffff; BG_COLOR.green = 0xffff; BG_COLOR.blue = 0xe000;
		gdk_colormap_alloc_color(colormap, &BG_COLOR, false, false);
		gdk_gc_set_foreground(myBackgroundGC, &BG_COLOR);

		myTextGC = gdk_gc_new(myPixmap);
		TX_COLOR.red = 0x8000; TX_COLOR.green = 0x8000; TX_COLOR.blue = 0xffff;
		gdk_colormap_alloc_color(colormap, &TX_COLOR, false, false);
		gdk_gc_set_foreground(myTextGC, &TX_COLOR);

		myFillGC = gdk_gc_new(myPixmap);
		FL_COLOR.red = 0xd000; FL_COLOR.green = 0xd000; FL_COLOR.blue = 0xffff;
		gdk_colormap_alloc_color(colormap, &FL_COLOR, false, false);
		gdk_gc_set_foreground(myFillGC, &FL_COLOR);

		PangoFontFamily **pangoFamilies;
  	int nFamilies;
  	pango_context_list_families (gtk_widget_get_pango_context (GTK_WIDGET(area)), &pangoFamilies, &nFamilies);
		for (int i = 0; i < nFamilies; i++) {
			myFontFamilies.push_back(pango_font_family_get_name(pangoFamilies[i]));
		}
		// TODO(?): sort list
		g_free(pangoFamilies);
	}

	gdk_draw_rectangle(myPixmap, myBackgroundGC, true, 0, 0, myWidth, myHeight);
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
}

void GtkPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	if (myFont != 0) {
		gdk_font_unref(myFont);
	}
	PangoFontDescription *pangoDescription = pango_font_description_new();
	pango_font_description_set_family(pangoDescription, family.c_str());
	pango_font_description_set_family(pangoDescription, "Monospace");
	pango_font_description_set_size(pangoDescription, size);
	myFont = gdk_font_from_description(pangoDescription);
	pango_font_description_free(pangoDescription);
	/*
	QFont font = myPainter->font();
	font.setFamily(family.c_str());
	font.setPointSize(size);
	font.setWeight(bold ? QFont::Bold : QFont::Normal);
	font.setItalic(italic);
	myPainter->setFont(font);
	*/
}

void GtkPaintContext::setColor(ZLColor color) {
	//myPainter->setPen(QColor(color.Red, color.Green, color.Blue));
}

void GtkPaintContext::setFillColor(ZLColor color) {
	//myPainter->setBrush(QColor(color.Red, color.Green, color.Blue));
}

int GtkPaintContext::wordWidth(const Word &word, int start, int length, bool addHyphenationSign) const {
	/*
	if ((start == 0) && (length == -1)) {
		return myPainter->fontMetrics().width(((QWord&)word).myValue);
	}
	QString subword = ((QWord&)word).myValue.mid(start, length);
	if (addHyphenationSign) {
		subword.append("-");
	}
	return myPainter->fontMetrics().width(subword);
	*/
	int w = 0;
	int len = word.length();
	for (int i = 0; i < len; i++) {
		w += gdk_char_width_wc(myFont, word.charAt(i));
	}
	return w;
}

int GtkPaintContext::spaceWidth() const {
	return gdk_text_width(myFont, " ", 1);
}

int GtkPaintContext::wordHeight() const {
	return 2 * gdk_char_height(myFont, 'X');
}

/*
void GtkPaintContext::drawQString(int x, int y, const QString &str, const Word::WordMark *mark, int shift) {
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
	y += topMargin().value();
	int len = word.length();
	for (int i = 0; i < len; i++) {
		GdkWChar wchar = word.charAt(i);
		gdk_draw_text_wc(myPixmap, myFont, myTextGC, x, y, &wchar, 1);
		x += gdk_char_width_wc(myFont, wchar);
	}
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
		myTextColor = TextColorOption.value();
		setColor(myTextColor);
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
