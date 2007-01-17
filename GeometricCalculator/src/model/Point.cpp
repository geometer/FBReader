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

#include <math.h>
#include <stdio.h>

std::map<const Point*const,Coordinates> Point::ourCoordinatesCache;

Point::Point(const std::string &name) : myName(name) {
}

Point::~Point() {
	ourCoordinatesCache.clear();
}

void Point::setName(const std::string &name) {
	myName = name;
}

const std::string &Point::name() const {
	return myName;
}

double Point::distance(double x, double y) const {
	const Coordinates coords = coordinates();
	double dx = x - coords.x;
	double dy = y - coords.y;
	return sqrt(dx * dx + dy * dy);
}

bool Point::canMove() const {
	return false;
}

void Point::moveTo_internal(double, double) {
}

void Point::moveTo(double x, double y) {
	if (canMove()) {
		moveTo_internal(x, y);
		ourCoordinatesCache.clear();
	}
}

void Point::moveTo(const Coordinates coords) {
	moveTo(coords.x, coords.y);
}

const Coordinates Point::coordinates() const {
	for (std::map<const Point*const,Coordinates>::iterator it = ourCoordinatesCache.begin(); !(it == ourCoordinatesCache.end()); it++){
		if ((*it).first == this) {
			return (*it).second;
		}
	}
	Coordinates coords = coordinates_internal();
	ourCoordinatesCache.insert(std::pair<const Point*const,Coordinates>(this, coords));
	return coords;
}

bool Point::isReal() const {
	return coordinates().isReal;
}

double Point::distance(const Coordinates coords) const {
	return Object::distance(coords);
}

double Point::distance(PointPtr point) const {
	return Object::distance(point);
}

Object::Type Point::rtti() const {
	return POINT;
}
