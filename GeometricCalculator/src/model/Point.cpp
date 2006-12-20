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
