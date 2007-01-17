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

#include "Circle.h"
#include "PointPtr.h"
#include "Point.h"
#include "../polynom/Polynom.h"
#include "../translator/Translator.h"

CircleByCenterAndPoint::CircleByCenterAndPoint(PointPtr center, PointPtr point) : myCenter(center), myPoint(point) {
	myDependsOn.push_back(myCenter);
	myDependsOn.push_back(myPoint);
}

CirclePtr CircleByCenterAndPoint::create(PointPtr center, PointPtr point) {
	return new CircleByCenterAndPoint(center, point);
}

const Coordinates CircleByCenterAndPoint::center() const {
	return myCenter->coordinates();
}

double CircleByCenterAndPoint::radius() const {
	return myPoint->distance(myCenter);
}

PointPtr CircleByCenterAndPoint::centerPoint() const {
	return myCenter;
}

const std::list<ObjectPtr> &CircleByCenterAndPoint::dependsOn() const {
	return myDependsOn;
}

static const std::string TYPE_VALUE = "CircleByCenterAndPoint";
static const std::string CENTER_KEY = "center";
static const std::string POINT_KEY = "point";

void CircleByCenterAndPoint::saveInfo(ObjectInfoMap &info) const {
	info.addAttribute(TYPE_KEY, TYPE_VALUE);
	info.addAttribute(CENTER_KEY, myCenter);
	info.addAttribute(POINT_KEY, myPoint);
}

CirclePtr CircleByCenterAndPoint::create(const ObjectInfoMap &info) {
	CirclePtr ptr;

	if (info.stringValueEqualsTo(TYPE_KEY, TYPE_VALUE)) {
		PointPtr center = info.getObjectValue(CENTER_KEY);
		PointPtr point = info.getObjectValue(POINT_KEY);
		if (!center.isNull() && !point.isNull()) {
			ptr = new CircleByCenterAndPoint(center, point);
		}
	}

	return ptr;
}

std::list<Polynom> CircleByCenterAndPoint::polynoms(Translator &translator) const {
	std::list<Polynom> list;
	Monom x0 = translator.monom(myCenter, "x");
	Monom y0 = translator.monom(myCenter, "y");
	Monom x = translator.monom(myPoint, "x");
	Monom y = translator.monom(myPoint, "y");
	Monom radius = translator.monom(*this, "radius");
	list.push_back((x - x0) * (x - x0) + (y - y0) * (y - y0) - radius * radius);
	return list;
}
