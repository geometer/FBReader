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
#include "../polynom/Polynom.h"
#include "../translator/Translator.h"

PointOnPerpendicular::PointOnPerpendicular(LinePtr line, PointPtr point, const std::string &name) : Point(name), myLine(line), myPoint(point) {
	myDependsOn.push_back(myLine);
	myDependsOn.push_back(myPoint);
}

PointPtr PointOnPerpendicular::create(LinePtr line, PointPtr point, const std::string &name) {
	return new PointOnPerpendicular(line, point, name);
}

const Coordinates PointOnPerpendicular::coordinates_internal() const {
	const Coordinates pointCoords = myPoint->coordinates();
	const Coordinates start = myLine->point(START)->coordinates();
	const Coordinates end = myLine->point(END)->coordinates();
	return Coordinates(
		pointCoords.x + start.y - end.y,
		pointCoords.y - start.x + end.x,
		myPoint->isReal() && myLine->point(START)->isReal() && myLine->point(END)->isReal());
}

const std::list<ObjectPtr> &PointOnPerpendicular::dependsOn() const {
	return myDependsOn;
}

static const std::string TYPE_VALUE = "PointOnPerpendicular";
static const std::string POINT_KEY = "point";
static const std::string LINE_KEY = "line";

void PointOnPerpendicular::saveInfo(ObjectInfoMap &info) const {
	info.addAttribute(TYPE_KEY, TYPE_VALUE);
	info.addAttribute(NAME_KEY, name());
	info.addAttribute(LINE_KEY, myLine);
	info.addAttribute(POINT_KEY, myPoint);
}

PointPtr PointOnPerpendicular::create(const ObjectInfoMap &info) {
	PointPtr ptr;

	if (info.stringValueEqualsTo(TYPE_KEY, TYPE_VALUE)) {
		const std::string &name = info.getStringValue(NAME_KEY);
		LinePtr line = info.getObjectValue(LINE_KEY);
		PointPtr point = info.getObjectValue(POINT_KEY);
		if (!line.isNull() && !point.isNull()) {
			ptr = new PointOnPerpendicular(line, point, name);
		}
	}

	return ptr;
}

std::list<Polynom> PointOnPerpendicular::polynoms(Translator &translator) const {
	// TODO: implement
	return std::list<Polynom>();
}
