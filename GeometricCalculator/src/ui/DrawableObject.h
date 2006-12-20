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
	DrawableCircle(int x, int y, double radius, const ZLColor &color, bool solid);
	void draw(ZLPaintContext &context);

private:
	const int myCenterX, myCenterY;
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
