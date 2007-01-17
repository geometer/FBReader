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

#include "Point.h"
#include "Line.h"
#include "Circle.h"
#include "../polynom/Polynom.h"
#include "../translator/Translator.h"

#include <math.h>

LineAndCircleCrossing::LineAndCircleCrossing(LinePtr line, CirclePtr circle, bool first, const std::string &name) : Point(name), myLine(line), myCircle(circle), myFirst(first) {
	myDependsOn.push_back(myLine);
	myDependsOn.push_back(myCircle);
}

LineAndCircleCrossing::~LineAndCircleCrossing() {
	myLine->disconnectPoint(myWeakPtr);
}

PointPtr LineAndCircleCrossing::create(LinePtr line, CirclePtr circle, bool first, const std::string &name) {
	LineAndCircleCrossing *p = new LineAndCircleCrossing(line, circle, first, name);
	PointPtr point = p;
	p->myWeakPtr = point;
	line->connectPoint(point);
	return point;
}

const Coordinates LineAndCircleCrossing::coordinates_internal() const {
	if (!myLine->isReal() || !myCircle->isReal()) {
		return Coordinates(0, 0, false);
	}

	// (x0 + alpha dX - x) ^ 2 + (y0 + alpha dY - y) ^ 2 = R ^ 2
	// (dX ^ 2 + dY ^ 2) alpha ^ 2
	// 		+ 2 * ((x0 - x) * dX + (y0 - y) * dY) alpha
	// 		+ (x0 - x) ^ 2 + (y0 - y) ^ 2 - R ^ 2 = 0

	const Coordinates start = myLine->point(START)->coordinates();
	const Coordinates end = myLine->point(END)->coordinates();
	const Coordinates center = myCircle->center();

	double doubledArea =
		fabs(center.x * start.y - center.y * start.x +
				 start.x * end.y - start.y * end.x +
				 end.x * center.y - end.y * center.x);
  double length = myLine->point(START)->distance(end);

	if (doubledArea / length > myCircle->radius() + 1e-4) {
		return Coordinates(0, 0, false);
	}

	double dX = end.x - start.x;
	double dY = end.y - start.y;

	double deltaX = start.x - center.x;
	double deltaY = start.y - center.y;

	double A = dX * dX + dY * dY;
	double B = dX * deltaX + dY * deltaY;
	double radius = myCircle->radius();
	double C = deltaX * deltaX + deltaY * deltaY - radius * radius;
	double discriminant = B * B - A * C;
	double root = 0;
	if (discriminant >= 0) {
		root = myFirst ? sqrt(discriminant) : -sqrt(discriminant);
	}
	double alpha = (root - B) / A;

	return Coordinates(start.x + alpha * dX, start.y + alpha * dY, true);
}

const std::list<ObjectPtr> &LineAndCircleCrossing::dependsOn() const {
	return myDependsOn;
}

static const std::string TYPE_VALUE = "LineAndCircleCrossing";
static const std::string LINE_KEY = "line";
static const std::string CIRCLE_KEY = "circle";
static const std::string FIRST_KEY = "first";

void LineAndCircleCrossing::saveInfo(ObjectInfoMap &info) const {
  info.addAttribute(TYPE_KEY, TYPE_VALUE);
  info.addAttribute(NAME_KEY, name());
  info.addAttribute(LINE_KEY, myLine);
  info.addAttribute(CIRCLE_KEY, myCircle);
  info.addAttribute(FIRST_KEY, myFirst);
}

PointPtr LineAndCircleCrossing::create(const ObjectInfoMap &info) {
	PointPtr ptr;

	if (info.stringValueEqualsTo(TYPE_KEY, TYPE_VALUE)) {
		const std::string &name = info.getStringValue(NAME_KEY);
		LinePtr line = info.getObjectValue(LINE_KEY);
		CirclePtr circle = info.getObjectValue(CIRCLE_KEY);
		bool first = info.getBooleanValue(FIRST_KEY);
		if (!line.isNull() && !circle.isNull()) {
			ptr = create(line, circle, first, name);
		}
	}

	return ptr;
}

std::list<Polynom> LineAndCircleCrossing::polynoms(Translator &translator) const {
	// TODO: implement
	return std::list<Polynom>();
}
