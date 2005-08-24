/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLOptions.h>
#include <abstract/ZLImageManager.h>

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
	ZLPaintContext() ZLVIEW_SECTION;

public:
	virtual ~ZLPaintContext() ZLVIEW_SECTION;

	int x() const ZLVIEW_SECTION;
	int y() const ZLVIEW_SECTION;

	void moveXTo(int x) ZLVIEW_SECTION;
	void moveX(int deltaX) ZLVIEW_SECTION;
	void moveYTo(int y) ZLVIEW_SECTION;
	void moveY(int deltaY) ZLVIEW_SECTION;

	void setLeftMargin(int margin) ZLVIEW_SECTION;
	void setRightMargin(int margin) ZLVIEW_SECTION;
	void setTopMargin(int margin) ZLVIEW_SECTION;
	void setBottomMargin(int margin) ZLVIEW_SECTION;

	int leftMargin() const ZLVIEW_SECTION;
	int rightMargin() const ZLVIEW_SECTION;
	int topMargin() const ZLVIEW_SECTION;
	int bottomMargin() const ZLVIEW_SECTION;

	virtual void clear(ZLColor color) ZLVIEW_SECTION = 0;

	virtual void setFont(const std::string &family, int size, bool bold, bool italic) ZLVIEW_SECTION = 0;
	virtual void setColor(ZLColor color, LineStyle style = SOLID_LINE) ZLVIEW_SECTION = 0;
	virtual void setFillColor(ZLColor color, FillStyle style = SOLID_FILL) ZLVIEW_SECTION = 0;

	virtual int width() const ZLVIEW_SECTION = 0;
	virtual int height() const ZLVIEW_SECTION = 0;
	
	virtual int stringWidth(const char *str, int len) const ZLVIEW_SECTION = 0;
	virtual int spaceWidth() const ZLVIEW_SECTION = 0;
	virtual int stringHeight() const ZLVIEW_SECTION = 0;
	virtual void drawString(int x, int y, const char *str, int len) ZLVIEW_SECTION = 0;

	int imageWidth(const ZLImageData &image) const ZLVIEW_SECTION;
	int imageHeight(const ZLImageData &image) const ZLVIEW_SECTION;
	virtual void drawImage(int x, int y, const ZLImageData &image) ZLVIEW_SECTION = 0;

	virtual void drawLine(int x0, int y0, int x1, int y1) ZLVIEW_SECTION = 0;
	virtual void fillRectangle(int x0, int y0, int x1, int y1) ZLVIEW_SECTION = 0;
	virtual void drawFilledCircle(int x, int y, int r) ZLVIEW_SECTION = 0;

	const std::vector<std::string> &fontFamilies() const ZLVIEW_SECTION;
	virtual const std::string realFontFamilyName(std::string &fontFamily) const ZLVIEW_SECTION = 0;

protected:
	virtual void fillFamiliesList(std::vector<std::string> &families) const ZLVIEW_SECTION = 0;

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
};

inline int ZLPaintContext::x() const { return myX; }
inline int ZLPaintContext::y() const { return myY; }

inline void ZLPaintContext::moveXTo(int x) { myX = x; }
inline void ZLPaintContext::moveX(int deltaX) { myX += deltaX; }
inline void ZLPaintContext::moveYTo(int y) { myY = y; }
inline void ZLPaintContext::moveY(int deltaY) { myY += deltaY; }

inline void ZLPaintContext::setLeftMargin(int margin) { myLeftMargin = margin; }
inline void ZLPaintContext::setRightMargin(int margin) { myRightMargin = margin; }
inline void ZLPaintContext::setTopMargin(int margin) { myTopMargin = margin; }
inline void ZLPaintContext::setBottomMargin(int margin) { myBottomMargin = margin; }

inline int ZLPaintContext::leftMargin() const { return myLeftMargin; }
inline int ZLPaintContext::rightMargin() const { return myRightMargin; }
inline int ZLPaintContext::topMargin() const { return myTopMargin; }
inline int ZLPaintContext::bottomMargin() const { return myBottomMargin; }

inline int ZLPaintContext::imageWidth(const ZLImageData &image) const { return image.width(); }
inline int ZLPaintContext::imageHeight(const ZLImageData &image) const { return image.height(); }

#endif /* __ZLPAINTCONTEXT_H__ */
