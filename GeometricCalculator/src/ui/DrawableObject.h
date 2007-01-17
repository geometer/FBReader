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

#ifndef __DRAWABLEOBJECT_H__
#define __DRAWABLEOBJECT_H__

#include <string>

#include <ZLPaintContext.h>

class DrawableObject {
	public:
		virtual void draw(ZLPaintContext &context) = 0;
		virtual ~DrawableObject();
};

class DrawableText : public DrawableObject {

public:
	DrawableText(int x, int y, const std::string &text, const std::string &family, int size, const ZLColor &color);
	void draw(ZLPaintContext &context);

private:
	const int myX, myY;
	const std::string myText;
	const std::string &myFamily;
	const int mySize;
	const ZLColor myColor;
};

class DrawablePoint : public DrawableObject {

public:
	DrawablePoint(int x, int y, int radius, const ZLColor &color, bool solid);
	void draw(ZLPaintContext &context);

private:
	const int myX, myY;
	const int myRadius;
	const ZLColor myColor;
	const bool mySolid;
};

class DrawableLine : public DrawableObject {

public:
	DrawableLine(int x0, int y0, int x1, int y1, const ZLColor &color, bool solid);
	void draw(ZLPaintContext &context);

private:
	const int myX0, myY0, myX1, myY1;
	const ZLColor myColor;
	const bool mySolid;
};

class DrawableCircle : public DrawableObject {

public:
	DrawableCircle(double x, double y, double radius, const ZLColor &color, bool solid);
	void draw(ZLPaintContext &context);

private:
	const double myCenterX, myCenterY;
	const double myRadius;
	const ZLColor myColor;
	const bool mySolid;
};

class DrawableRuler : public DrawableObject {

public:
	DrawableRuler(int x0, int y0, int x1, int y1, const ZLColor &color, bool solid);
	void draw(ZLPaintContext &context);

private:
	const int myX0, myY0, myX1, myY1;
	const ZLColor myColor;
	const bool mySolid;
};

#endif /* __DRAWABLEOBJECT_H__ */
