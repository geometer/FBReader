#include "Point.h"
#include "Line.h"
#include "../polynom/Polynom.h"
#include "../translator/Translator.h"

PointOnTheLine::PointOnTheLine(PointPtr point0, PointPtr point1, LinePtr line, double alpha, const std::string &name) : Point(name), myPoint0(point0), myPoint1(point1), myAlpha(alpha), myIsFixed(false) {
	myLine = line;
	myDependsOn.push_back(myPoint0);
	myDependsOn.push_back(myPoint1);
}

PointOnTheLine::~PointOnTheLine() {
	if (!myLine.isNull()) {
		myLine->disconnectPoint(myWeakPtr);
	}
}

PointPtr PointOnTheLine::create(PointPtr point0, PointPtr point1, LinePtr line, double alpha, const std::string &name) {
	PointOnTheLine *p = new PointOnTheLine(point0, point1, line, alpha, name);
	PointPtr point = p;
	p->myWeakPtr = point;
	if (!line.isNull()) {
		line->connectPoint(point);
	}
	return point;
}

const Coordinates PointOnTheLine::coordinates_internal() const {
	const Coordinates start = myPoint0->coordinates();
	const Coordinates end = myPoint1->coordinates();
	return Coordinates(end.x + myAlpha * (start.x - end.x), end.y + myAlpha * (start.y - end.y), myPoint0->isReal() && myPoint1->isReal());
}

bool PointOnTheLine::canMove() const {
	return !isFixed();
}

void PointOnTheLine::moveTo_internal(double x, double y) {
	if (canMove()) {
		const Coordinates start = myPoint0->coordinates();
		const Coordinates end = myPoint1->coordinates();
		double lineDX = start.x - end.x;
		double lineDY = start.y - end.y;
		if ((lineDX == 0) && (lineDY == 0)) {
			return;
		}

		double dx = x - end.x;
		double dy = y - end.y;
		myAlpha = (lineDX * dx + lineDY * dy) / (lineDX * lineDX + lineDY * lineDY);
	}
}

bool PointOnTheLine::isFixed() const {
	return myIsFixed;
}

void PointOnTheLine::setFixed(bool fixed) {
	myIsFixed = fixed;
}

const std::list<ObjectPtr> &PointOnTheLine::dependsOn() const {
	return myDependsOn;
}

static const std::string TYPE_VALUE = "PointOnTheLine";
static const std::string ALPHA_KEY = "alpha";
static const std::string LINE_KEY = "line";
static const std::string POINT0_KEY = "point0";
static const std::string POINT1_KEY = "point1";
static const std::string FIXED_KEY = "fixed";

void PointOnTheLine::saveInfo(ObjectInfoMap &info) const {
	info.addAttribute(TYPE_KEY, TYPE_VALUE);
	info.addAttribute(NAME_KEY, name());
	info.addAttribute(POINT0_KEY, myPoint0);
	info.addAttribute(POINT1_KEY, myPoint1);
	if (!myLine.isNull()) {
		info.addAttribute(LINE_KEY, myLine);
	}
	info.addAttribute(ALPHA_KEY, myAlpha);
	info.addAttribute(FIXED_KEY, isFixed());
}

PointPtr PointOnTheLine::create(const ObjectInfoMap &info) {
	PointPtr ptr;

	if (info.stringValueEqualsTo(TYPE_KEY, TYPE_VALUE)) {
		const std::string &name = info.getStringValue(NAME_KEY);
		PointPtr point0 = info.getObjectValue(POINT0_KEY);
		PointPtr point1 = info.getObjectValue(POINT1_KEY);
		LinePtr line = info.getObjectValue(LINE_KEY);
		double alpha = info.getDoubleValue(ALPHA_KEY);
		bool fixed = info.getBooleanValue(FIXED_KEY);
		if (!point0.isNull() && !point1.isNull()) {
			ptr = create(point0, point1, line, alpha, name);
			((PointOnTheLine&)*ptr).setFixed(fixed);
		}
	}

	return ptr;
}

std::list<Polynom> PointOnTheLine::polynoms(Translator &translator) const {
	std::list<Polynom> list;
	Monom x = translator.monom(*this, "x");
	Monom x0 = translator.monom(myPoint0, "x");
	Monom x1 = translator.monom(myPoint1, "x");
	Monom y = translator.monom(*this, "y");
	Monom y0 = translator.monom(myPoint0, "y");
	Monom y1 = translator.monom(myPoint1, "y");
	if (isFixed()) {
		// TODO: use alpha value
	  list.push_back(x0 + x1 - 2 * x);
	  list.push_back(y0 + y1 - 2 * y);
	} else {
		Monom alpha = translator.monom(*this, "alpha");
		list.push_back(alpha * x0 + (1 - alpha) * x1 - x);
		list.push_back(alpha * y0 + (1 - alpha) * y1 - y);
	}
	return list;
}
