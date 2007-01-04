#include <math.h>

#include "DrawableObject.h"

DrawableObject::~DrawableObject() {
}

DrawableText::DrawableText(int x, int y, const std::string &text, const std::string &family, int size, const ZLColor &color) : myX(x), myY(y), myText(text), myFamily(family), mySize(size), myColor(color) {
}

void DrawableText::draw(ZLPaintContext &context) {
	context.setFont(myFamily, mySize, false, false);
	context.setColor(myColor);
	context.drawString(myX, myY, myText.c_str(), myText.length());
}

DrawablePoint::DrawablePoint(int x, int y, int radius, const ZLColor &color, bool solid) : myX(x), myY(y), myRadius(radius), myColor(color), mySolid(solid) {
}

void DrawablePoint::draw(ZLPaintContext &context) {
	context.setColor(myColor);
	context.setFillColor(myColor, mySolid ? ZLPaintContext::SOLID_FILL : ZLPaintContext::HALF_FILL);
	context.drawFilledCircle(myX, myY, myRadius);
}

DrawableLine::DrawableLine(int x0, int y0, int x1, int y1, const ZLColor &color, bool solid) : myX0(x0), myY0(y0), myX1(x1), myY1(y1), myColor(color), mySolid(solid) {
}

void DrawableLine::draw(ZLPaintContext &context) {
	context.setColor(myColor, mySolid ? ZLPaintContext::SOLID_LINE : ZLPaintContext::DASH_LINE);
	context.drawLine(myX0, myY0, myX1, myY1);
}

DrawableCircle::DrawableCircle(double x, double y, double radius, const ZLColor &color, bool solid) : myCenterX(x), myCenterY(y), myRadius(radius), myColor(color), mySolid(solid) {
}

const uint MAX_POINT_NUMBER = 50;
static bool SIN_CALCULATED = false;
static double SIN[MAX_POINT_NUMBER + 1];
static double COS[MAX_POINT_NUMBER + 1];

void DrawableCircle::draw(ZLPaintContext &context) {
	if (!SIN_CALCULATED) {
		for (uint i = 0; i <= MAX_POINT_NUMBER; i++) {
			double angle = 2 * 3.1415926536 * i / MAX_POINT_NUMBER;
			SIN[i] = sin(angle);
			COS[i] = cos(angle);
		}
		SIN_CALCULATED = true;
	}

	context.setColor(myColor, mySolid ? ZLPaintContext::SOLID_LINE : ZLPaintContext::DASH_LINE);
	for (uint i = 0; i < MAX_POINT_NUMBER; i++) {
		context.drawLine((int)(myCenterX + SIN[i] * myRadius + .5), (int)(myCenterY + COS[i] * myRadius + .5),
										 (int)(myCenterX + SIN[i + 1] * myRadius + .5), (int)(myCenterY + COS[i + 1] * myRadius + .5));
	}
}

DrawableRuler::DrawableRuler(int x0, int y0, int x1, int y1, const ZLColor &color, bool solid) : myX0(x0), myY0(y0), myX1(x1), myY1(y1), myColor(color), mySolid(solid) {
}

void DrawableRuler::draw(ZLPaintContext &context) {
	context.setColor(myColor, mySolid ? ZLPaintContext::SOLID_LINE : ZLPaintContext::DASH_LINE);

	double dx = myX0 - myX1;
	double dy = myY0 - myY1;

	double length = sqrt(dx * dx + dy * dy);
	if (length < 1) {
		return;
	}

	dx = dx / length * 30;
	dy = dy / length * 30;
	bool signChanged = dx > 0;
	if (signChanged) {
		dx = -dx;
		dy = -dy;
	}

	context.drawLine(myX0, myY0, (int)(myX0 - dy), (int)(myY0 + dx));
	context.drawLine(myX1, myY1, (int)(myX1 - dy), (int)(myY1 + dx));

	dx /= 1.2;
	dy /= 1.2;
	context.drawLine((int)(myX0 - dy), (int)(myY0 + dx), (int)(myX1 - dy), (int)(myY1 + dx));
}
