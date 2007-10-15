/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLPAINTCONTEXT_H__
#define __ZLPAINTCONTEXT_H__

#include <vector>
#include <string>

#include <ZLColor.h>
#include <ZLImageManager.h>

class ZLPaintContext {

public:
	enum LineStyle {
		SOLID_LINE,
		DASH_LINE,
	};

	enum FillStyle {
		SOLID_FILL,
		HALF_FILL,
	};
	
protected:
	ZLPaintContext();

public:
	virtual ~ZLPaintContext();

	int x() const;
	int y() const;

	void moveXTo(int x);
	void moveX(int deltaX);
	void moveYTo(int y);
	void moveY(int deltaY);

	void setLeftMargin(int margin);
	void setRightMargin(int margin);
	void setTopMargin(int margin);
	void setBottomMargin(int margin);

	int leftMargin() const;
	int rightMargin() const;
	int topMargin() const;
	int bottomMargin() const;

	virtual void clear(ZLColor color) = 0;

	virtual void setFont(const std::string &family, int size, bool bold, bool italic) = 0;
	virtual void setColor(ZLColor color, LineStyle style = SOLID_LINE) = 0;
	virtual void setFillColor(ZLColor color, FillStyle style = SOLID_FILL) = 0;

	virtual int width() const = 0;
	virtual int height() const = 0;
	
	virtual int stringWidth(const char *str, int len) const = 0;
	virtual int spaceWidth() const = 0;
	virtual int stringHeight() const = 0;
	virtual int descent() const = 0;
	virtual void drawString(int x, int y, const char *str, int len) = 0;

	int imageWidth(const ZLImageData &image) const;
	int imageHeight(const ZLImageData &image) const;
	virtual void drawImage(int x, int y, const ZLImageData &image) = 0;

	virtual void drawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void fillRectangle(int x0, int y0, int x1, int y1) = 0;
	virtual void drawFilledCircle(int x, int y, int r) = 0;

	const std::vector<std::string> &fontFamilies() const;
	virtual const std::string realFontFamilyName(std::string &fontFamily) const = 0;

protected:
	virtual void fillFamiliesList(std::vector<std::string> &families) const = 0;

private:
	int myX, myY;

	int myLeftMargin;
	int myRightMargin;
	int myTopMargin;
	int myBottomMargin;

	mutable std::vector<std::string> myFamilies;

private:
	ZLPaintContext(const ZLPaintContext&);
	const ZLPaintContext& operator = (const ZLPaintContext&);

friend class ZLPartialPaintContext;
};

inline int ZLPaintContext::x() const { return myX; }
inline int ZLPaintContext::y() const { return myY; }

inline void ZLPaintContext::moveXTo(int x) { myX = x; }
inline void ZLPaintContext::moveX(int deltaX) { myX += deltaX; }
inline void ZLPaintContext::moveYTo(int y) { myY = y; }
inline void ZLPaintContext::moveY(int deltaY) { myY += deltaY; }

inline int ZLPaintContext::leftMargin() const { return myLeftMargin; }
inline int ZLPaintContext::rightMargin() const { return myRightMargin; }
inline int ZLPaintContext::topMargin() const { return myTopMargin; }
inline int ZLPaintContext::bottomMargin() const { return myBottomMargin; }

inline int ZLPaintContext::imageWidth(const ZLImageData &image) const { return image.width(); }
inline int ZLPaintContext::imageHeight(const ZLImageData &image) const { return image.height(); }

#endif /* __ZLPAINTCONTEXT_H__ */
