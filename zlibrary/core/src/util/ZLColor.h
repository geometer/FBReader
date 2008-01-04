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

#ifndef __ZLCOLOR_H__
#define __ZLCOLOR_H__

struct ZLColor {
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;

	ZLColor(unsigned char r, unsigned char g, unsigned char b);
	ZLColor(long longValue = 0);
	long intValue();

	bool operator == (const ZLColor &color) const;
	bool operator != (const ZLColor &color) const;
};

inline ZLColor::ZLColor(unsigned char r, unsigned char g, unsigned char b) : Red(r), Green(g), Blue(b) {}
inline ZLColor::ZLColor(long longValue) : Red(longValue >> 16), Green(longValue >> 8), Blue(longValue) {}
inline long ZLColor::intValue() { return (((long)Red) << 16) + (((long)Green) << 8) + Blue; }
inline bool ZLColor::operator == (const ZLColor &color) const { return (Red == color.Red) && (Green == color.Green) && (Blue == color.Blue); }
inline bool ZLColor::operator != (const ZLColor &color) const { return !operator==(color); }

#endif /* __ZLCOLOR_H__ */
