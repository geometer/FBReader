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
#include "Circle.h"
#include "../polynom/Polynom.h"
#include "../translator/Translator.h"

#include <math.h>

#ifndef M_PI
#define M_PI   3.14159265358979323846
#endif

PointOnTheCircle::PointOnTheCircle(CirclePtr circle, double angle, const std::string &name) : Point(name), myCircle(circle), myAngle(angle) {
	myDependsOn.push_back(myCircle);
}

PointPtr PointOnTheCircle::create(CirclePtr circle, double angle, const std::string &name) {
	return new PointOnTheCircle(circle, angle, name);
}

const Coordinates PointOnTheCircle::coordinates_internal() const {
	const Coordinates center = myCircle->center();
	double radius = myCircle->radius();
	return Coordinates(center.x + radius * sin(myAngle), center.y + radius * cos(myAngle), myCircle->isReal());
}

bool PointOnTheCircle::canMove() const {
	return true;
}

void PointOnTheCircle::moveTo_internal(double x, double y) {
	const Coordinates center = myCircle->center();
	double dx = center.x - x;
	double dy = center.y - y;
	if ((dx == 0) && (dy == 0)) {
		return;
	}

	if (fabs(dy) > fabs(dx)) {
		myAngle = atan(dx / dy);

		if (dy > 0) {
			myAngle = M_PI + myAngle;
		}
	} else {
		myAngle = M_PI / 2 - atan(dy / dx);

		if (dx > 0) {
			myAngle = M_PI + myAngle;
		}
	}
}

const std::list<ObjectPtr> &PointOnTheCircle::dependsOn() const {
	return myDependsOn;
}

static const std::string TYPE_VALUE = "PointOnTheCircle";
static const std::string ANGLE_KEY = "angle";
static const std::string LINE_KEY = "circle";

void PointOnTheCircle::saveInfo(ObjectInfoMap &info) const {
	info.addAttribute(TYPE_KEY, TYPE_VALUE);
	info.addAttribute(NAME_KEY, name());
	info.addAttribute(LINE_KEY, myCircle);
	info.addAttribute(ANGLE_KEY, myAngle);
}

PointPtr PointOnTheCircle::create(const ObjectInfoMap &info) {
	PointPtr ptr;

	if (info.stringValueEqualsTo(TYPE_KEY, TYPE_VALUE)) {
		const std::string &name = info.getStringValue(NAME_KEY);
		CirclePtr circle = info.getObjectValue(LINE_KEY);
		double angle = info.getDoubleValue(ANGLE_KEY);
		if (!circle.isNull()) {
			ptr = new PointOnTheCircle(circle, angle, name);
		}
	}

	return ptr;
}

std::list<Polynom> PointOnTheCircle::polynoms(Translator &translator) const {
	std::list<Polynom> list;
	PointPtr circleCenter = myCircle->centerPoint();
	if (!circleCenter.isNull()) {
		Monom x0 = translator.monom(circleCenter, "x");
		Monom y0 = translator.monom(circleCenter, "y");
		Monom radius = translator.monom(circleCenter, "radius");
		Monom x = translator.monom(*this, "x");
		Monom y = translator.monom(*this, "y");
		list.push_back((x - x0) * (x - x0) + (y - y0) * (y - y0) - radius * radius);
	}
	return list;
}
