/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

	virtual void clear(ZLColor color) = 0;

	virtual void setFont(const std::string &family, int size, bool bold, bool italic) = 0;
	virtual void setColor(ZLColor color, LineStyle style = SOLID_LINE) = 0;
	virtual void setFillColor(ZLColor color, FillStyle style = SOLID_FILL) = 0;

	virtual int width() const = 0;
	virtual int height() const = 0;
	
	virtual int stringWidth(const char *str, int len, bool rtl) const = 0;
	virtual int spaceWidth() const = 0;
	virtual int stringHeight() const = 0;
	virtual int descent() const = 0;
	virtual void drawString(int x, int y, const char *str, int len, bool rtl) = 0;

	enum ScalingType {
		SCALE_FIT_TO_SIZE,
		SCALE_REDUCE_SIZE
	};

	int imageWidth(const ZLImageData &image) const;
	int imageHeight(const ZLImageData &image) const;
	int imageWidth(const ZLImageData &image, int width, int height, ScalingType type) const;
	int imageHeight(const ZLImageData &image, int width, int height, ScalingType type) const;
	virtual void drawImage(int x, int y, const ZLImageData &image) = 0;
	virtual void drawImage(int x, int y, const ZLImageData &image, int width, int height, ScalingType type) = 0;

	virtual void drawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void fillRectangle(int x0, int y0, int x1, int y1) = 0;
	virtual void drawFilledCircle(int x, int y, int r) = 0;

	const std::vector<std::string> &fontFamilies() const;
	virtual const std::string realFontFamilyName(std::string &fontFamily) const = 0;

protected:
	virtual void fillFamiliesList(std::vector<std::string> &families) const = 0;

private:
	mutable std::vector<std::string> myFamilies;

private:
	ZLPaintContext(const ZLPaintContext&);
	const ZLPaintContext &operator = (const ZLPaintContext&);

friend class ZLMirroredPaintContext;
};

inline int ZLPaintContext::imageWidth(const ZLImageData &image) const { return image.width(); }
inline int ZLPaintContext::imageHeight(const ZLImageData &image) const { return image.height(); }

#endif /* __ZLPAINTCONTEXT_H__ */
