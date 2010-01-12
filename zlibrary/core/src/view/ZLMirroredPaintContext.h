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

#ifndef __ZLMIRROREDPAINTCONTEXT_H__
#define __ZLMIRROREDPAINTCONTEXT_H__

#include <vector>
#include <string>

#include <ZLColor.h>
#include <ZLPaintContext.h>

class ZLMirroredPaintContext : public ZLPaintContext {

public:
	ZLMirroredPaintContext(ZLPaintContext &base);

public:
	int mirroredX(int x) const;

private:
	void clear(ZLColor color);

	void setFont(const std::string &family, int size, bool bold, bool italic);
	void setColor(ZLColor color, LineStyle style);
	void setFillColor(ZLColor color, FillStyle style);

	int width() const;
	int height() const;
	
	int stringWidth(const char *str, int len, bool rtl) const;
	int spaceWidth() const;
	int stringHeight() const;
	int descent() const;
	void drawString(int x, int y, const char *str, int len, bool rtl);

	void drawImage(int x, int y, const ZLImageData &image);
	void drawImage(int x, int y, const ZLImageData &image, int width, int height, ScalingType type);

	void drawLine(int x0, int y0, int x1, int y1);
	void fillRectangle(int x0, int y0, int x1, int y1);
	void drawFilledCircle(int x, int y, int r);

	const std::string realFontFamilyName(std::string &fontFamily) const;

	void fillFamiliesList(std::vector<std::string> &families) const;

private:
	ZLPaintContext &myBase;
};

inline int ZLMirroredPaintContext::mirroredX(int x) const {
	return myBase.width() - x - 1;
}

#endif /* __ZLMIRROREDPAINTCONTEXT_H__ */
