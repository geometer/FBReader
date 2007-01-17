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

FourthParallelogramPoint::FourthParallelogramPoint(PointPtr first, PointPtr second, PointPtr third, const std::string &name) : Point(name), myFirst(first), mySecond(second), myThird(third) {
	myDependsOn.push_back(first);
	myDependsOn.push_back(second);
	myDependsOn.push_back(third);
}

PointPtr FourthParallelogramPoint::create(PointPtr first, PointPtr second, PointPtr third, const std::string &name) {
	return new FourthParallelogramPoint(first, second, third, name);
}

const Coordinates FourthParallelogramPoint::coordinates_internal() const {
	const Coordinates coords1 = myFirst->coordinates();
	const Coordinates coords2 = mySecond->coordinates();
	const Coordinates coords3 = myThird->coordinates();
	return Coordinates(
		coords1.x - coords2.x + coords3.x,
		coords1.y - coords2.y + coords3.y,
		myFirst->isReal() && mySecond->isReal() && myThird->isReal()
	);
}

const std::list<ObjectPtr> &FourthParallelogramPoint::dependsOn() const {
	return myDependsOn;
}

static const std::string TYPE_VALUE = "FourthParallelogramPoint";
static const std::string FIRST_KEY = "first";
static const std::string SECOND_KEY = "second";
static const std::string THIRD_KEY = "third";

void FourthParallelogramPoint::saveInfo(ObjectInfoMap &info) const {
	info.addAttribute(TYPE_KEY, TYPE_VALUE);
	info.addAttribute(NAME_KEY, name());
	info.addAttribute(FIRST_KEY, myFirst);
	info.addAttribute(SECOND_KEY, mySecond);
	info.addAttribute(THIRD_KEY, myThird);
}

PointPtr FourthParallelogramPoint::create(const ObjectInfoMap &info) {
	PointPtr ptr;

	if (info.stringValueEqualsTo(TYPE_KEY, TYPE_VALUE)) {
		const std::string &name = info.getStringValue(NAME_KEY);
		PointPtr first = info.getObjectValue(FIRST_KEY);
		PointPtr second = info.getObjectValue(SECOND_KEY);
		PointPtr third = info.getObjectValue(THIRD_KEY);
		if (!first.isNull() && !second.isNull() && !third.isNull()) {
			ptr = new FourthParallelogramPoint(first, second, third, name);
		}
	}

	return ptr;
}

std::list<Polynom> FourthParallelogramPoint::polynoms(Translator &translator) const {
	std::list<Polynom> list;
	list.push_back(
			translator.monom(myFirst, "x") +
			translator.monom(myThird, "x") -
			translator.monom(mySecond, "x") -
			translator.monom(*this, "x"));
	list.push_back(
			translator.monom(myFirst, "y") +
			translator.monom(myThird, "y") -
			translator.monom(mySecond, "y") -
			translator.monom(*this, "y"));
	return list;
}
