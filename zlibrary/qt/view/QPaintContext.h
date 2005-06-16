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

#ifndef __QPAINTCONTEXT_H__
#define __QPAINTCONTEXT_H__

#include <map>

#include <abstract/ZLPaintContext.h>

class QPixmap;
class QImage;

class QPaintContext : public ZLPaintContext {

public:
	QPaintContext();
	~QPaintContext();
	void removeCaches();

	const QPixmap &pixmap() const { return *myPixmap; }

	void setSize(int w, int h);

	int width() const;
	int height() const;

	void clear();

	void fillFamiliesList(std::vector<std::string> &families) const;
	const std::string realFontFamilyName(std::string &fontFamily) const;

	void setFont(const std::string &family, int size, bool bold, bool italic);
	void setColor(ZLColor color);
	void setFillColor(ZLColor color);

	int stringWidth(const char *str, int len) const;
	int spaceWidth() const;
	int stringHeight() const;
	void drawString(int x, int y, const char *str, int len);

	int imageWidth(const ZLImage &image) const;
	int imageHeight(const ZLImage &image) const;
	void drawImage(int x, int y, const ZLImage &image);

	void drawLine(int x0, int y0, int x1, int y1);
	void fillRectangle(int x0, int y0, int x1, int y1);

private:
	QImage &qImage(const ZLImage &image) const;

private:
	QPainter *myPainter;
	QPixmap *myPixmap;
	mutable std::map<const ZLImage*,QImage*> myImageCache;
	mutable int mySpaceWidth;
};

#endif /* __QPAINTCONTEXT_H__ */
