#include "Point.h"
#include "Line.h"
#include "../polynom/Polynom.h"
#include "../translator/Translator.h"

#include <math.h>

LinesCrossing::LinesCrossing(LinePtr line0, LinePtr line1, const std::string &name) : Point(name), myLine0(line0), myLine1(line1) {
	myDependsOn.push_back(myLine0);
	myDependsOn.push_back(myLine1);
}

LinesCrossing::~LinesCrossing() {
	myLine0->disconnectPoint(myWeakPtr);
	myLine1->disconnectPoint(myWeakPtr);
}

PointPtr LinesCrossing::create(LinePtr line0, LinePtr line1, const std::string &name) {
	LinesCrossing *p = new LinesCrossing(line0, line1, name);
	PointPtr point = p;
	p->myWeakPtr = point;
	line0->connectPoint(point);
	line1->connectPoint(point);
	return point;
}

const Coordinates LinesCrossing::coordinates_internal() const {
	if (!myLine0->isReal() || !myLine1->isReal()) {
		return Coordinates(0, 0, false);
	}

	const Coordinates start0 = myLine0->point(START)->coordinates();
	const Coordinates end0 = myLine0->point(END)->coordinates();
	double x0 = start0.x;
	double y0 = start0.y;
	double dx0 = start0.x - end0.x;
	double dy0 = start0.y - end0.y;

	const Coordinates start1 = myLine1->point(START)->coordinates();
	const Coordinates end1 = myLine1->point(END)->coordinates();
	double x1 = start1.x;
	double y1 = start1.y;
	double dx1 = start1.x - end1.x;
	double dy1 = start1.y - end1.y;

	// x0 + A dx0 = x1 + B dx1
	// y0 + A dy0 = y1 + B dy1

	// A dx0 - B dx1 = x1 - x0
	// A dy0 - B dy1 = y1 - y0

	// A dx0 dy1 - B dx1 dy1 = x1 dy1 - x0 dy1
	// A dy0 dx1 - B dy1 dx1 = y1 dx1 - y0 dx1

	// A (dx0 dy1 - dy0 dx1) = x1 dy1 - x0 dy1 - y1 dx1 + y0 dx1

	double area = dx0 * dy1 - dx1 * dy0;
	if (area == 0) {
		return Coordinates(0, 0, false);
	}

	double alpha = ((x1 - x0) * dy1 + (y0 - y1) * dx1) / area;
	return Coordinates(x0 + alpha * dx0, y0 + alpha * dy0, true);
}

const std::list<ObjectPtr> &LinesCrossing::dependsOn() const {
	return myDependsOn;
}

static const std::string TYPE_VALUE = "LinesCrossing";
static const std::string LINE0_KEY = "line0";
static const std::string LINE1_KEY = "line1";

void LinesCrossing::saveInfo(ObjectInfoMap &info) const {
	info.addAttribute(TYPE_KEY, TYPE_VALUE);
	info.addAttribute(NAME_KEY, name());
	info.addAttribute(LINE0_KEY, myLine0);
	info.addAttribute(LINE1_KEY, myLine1);
}

PointPtr LinesCrossing::create(const ObjectInfoMap &info) {
	PointPtr ptr;

	if (info.stringValueEqualsTo(TYPE_KEY, TYPE_VALUE)) {
		const std::string &name = info.getStringValue(NAME_KEY);
		LinePtr line0 = info.getObjectValue(LINE0_KEY);
		LinePtr line1 = info.getObjectValue(LINE1_KEY);
		if (!line0.isNull() && !line1.isNull()) {
			ptr = create(line0, line1, name);
		}
	}

	return ptr;
}

std::list<Polynom> LinesCrossing::polynoms(Translator &translator) const {
	// TODO: implement
	return std::list<Polynom>();
}
