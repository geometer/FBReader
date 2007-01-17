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
#include "../polynom/Polynom.h"
#include "../translator/Translator.h"

IndependentPoint::IndependentPoint(int x, int y, const std::string &name) : Point(name), myCoordinates(x, y, true) {
}

PointPtr IndependentPoint::create(int x, int y, const std::string &name) {
	return new IndependentPoint(x, y, name);
}

bool IndependentPoint::canMove() const {
	return true;
}

void IndependentPoint::moveTo_internal(double x, double y) {
	myCoordinates.x = x;
	myCoordinates.y = y;
}

const Coordinates IndependentPoint::coordinates_internal() const {
	return myCoordinates;
}

static const std::string TYPE_VALUE = "IndependentPoint";
static const std::string X_KEY = "x";
static const std::string Y_KEY = "y";

void IndependentPoint::saveInfo(ObjectInfoMap &info) const {
	info.addAttribute(TYPE_KEY, TYPE_VALUE);
	info.addAttribute(NAME_KEY, name());
	info.addAttribute(X_KEY, (int)(myCoordinates.x + .5));
	info.addAttribute(Y_KEY, (int)(myCoordinates.y + .5));
}

PointPtr IndependentPoint::create(const ObjectInfoMap &info) {
	PointPtr ptr;

	if (info.stringValueEqualsTo(TYPE_KEY, TYPE_VALUE)) {
		const std::string &name = info.getStringValue(NAME_KEY);
		int x = info.getIntValue(X_KEY);
		int y = info.getIntValue(Y_KEY);
		ptr = new IndependentPoint(x, y, name);
	}

	return ptr;
}

std::list<Polynom> IndependentPoint::polynoms(Translator &translator) const {
	return std::list<Polynom>();
}
