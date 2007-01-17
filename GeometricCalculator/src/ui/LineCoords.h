/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __LINECOORDS_H__
#define __LINECOORDS_H__

#include <shared_ptr.h>

class PointPtr;
class LinePtr;

class LineCoords {
	private:
		double myXStart, myYStart, myXEnd, myYEnd;

	public:
		LineCoords(LinePtr line, const int width, const int height);
		LineCoords(PointPtr start, PointPtr end);

		const int XStart() const;
		const int YStart() const;
		const int XEnd() const;
		const int YEnd() const;
};

typedef shared_ptr<LineCoords> LineCoordsPtr;

#endif /* __LINECOORDS_H__ */
