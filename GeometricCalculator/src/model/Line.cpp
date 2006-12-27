#include "Point.h"
#include "Line.h"

#include <math.h>

const std::list<PointWeakPtr> &Line::points() const {
	static const std::list<PointWeakPtr> EMPTY_POINT_LIST;
	return EMPTY_POINT_LIST;
}

bool Line::contains(const PointPtr pt) const {
	if (pt.isNull()) {
		return false;
	}

	if ((pt == point(START)) || (pt == point(END))) {
		return true;
	}

	const std::list<PointWeakPtr> &morePoints = points();
	for (std::list<PointWeakPtr>::const_iterator it = morePoints.begin(); it != morePoints.end(); it++) {
		if (*it == pt) {
			return true;
		}
	}

	return false;
}

double Line::distance(double x, double y) const {
	const Coordinates start = point(START)->coordinates();
	const Coordinates end = point(END)->coordinates();

	if ((start.x == end.x) && (start.y == end.y)) {
		return point(START)->distance(x, y);
	}

	if ((kind() == HALFLINE) || (kind() == SEGMENT)) {
		double dx1 = x - start.x;
		double dy1 = y - start.y;
		double dx2 = end.x - start.x;
		double dy2 = end.y - start.y;
		if (dx1 * dx2 + dy1 * dy2 < 0) {
		  return point(START)->distance(x, y);
		}
	}

	if (kind() == SEGMENT) {
		double dx1 = x - end.x;
		double dy1 = y - end.y;
		double dx2 = start.x - end.x;
		double dy2 = start.y - end.y;
		if (dx1 * dx2 + dy1 * dy2 < 0) {
		  return point(END)->distance(x, y);
		}
	}

	double doubledArea =
		fabs(x * start.y - y * start.x +
				 start.x * end.y - start.y * end.x +
				 end.x * y - end.y * x);
  double length = point(START)->distance(end.x, end.y);
	return doubledArea / length;
}

double Line::distance(const Coordinates coords) const {
	return Object::distance(coords);
}

double Line::distance(const PointPtr point) const {
	return Object::distance(point);
}

Object::Type Line::rtti() const {
	return LINE;
}
