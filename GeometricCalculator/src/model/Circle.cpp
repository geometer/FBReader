#include <math.h>

#include "Circle.h"
#include "PointPtr.h"

PointPtr Circle::centerPoint() const {
	return PointPtr();
}

double Circle::distance(double x, double y) const {
	const Coordinates coords = center();
	double dx = x - coords.x;
	double dy = y - coords.y;
	return fabs(sqrt(dx * dx + dy * dy) - radius());
}

double Circle::distance(const Coordinates coords) const {
	return Object::distance(coords);
}

double Circle::distance(PointPtr point) const {
	return Object::distance(point);
}
