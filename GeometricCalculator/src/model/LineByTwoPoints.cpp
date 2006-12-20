#include "Point.h"
#include "Line.h"
#include "../polynom/Polynom.h"
#include "../translator/Translator.h"

LineByTwoPoints::LineByTwoPoints(const PointPtr startPoint, const PointPtr endPoint, LineKind kind) : myStartPoint(startPoint), myEndPoint(endPoint) {
	myKind = kind;
	myDependsOn.push_back(myStartPoint);
	myDependsOn.push_back(myEndPoint);
}

LinePtr LineByTwoPoints::create(const PointPtr startPoint, const PointPtr endPoint, LineKind kind) {
	return new LineByTwoPoints(startPoint, endPoint, kind);
}

const PointPtr LineByTwoPoints::point(LineEnd end) const {
	if (end == START) {
		return myStartPoint;
	} else {
		return myEndPoint;
	}
}

const std::list<PointWeakPtr> &LineByTwoPoints::points() const {
	return myPoints;
}

void LineByTwoPoints::connectPoint(const PointPtr point) {
	myPoints.push_back(point);
}

void LineByTwoPoints::disconnectPoint(const PointWeakPtr point) {
	myPoints.remove(point);
}

LineKind LineByTwoPoints::kind() const {
	return myKind;
}

bool LineByTwoPoints::isReal() const {
	const Coordinates start = point(START)->coordinates();
	const Coordinates end = point(END)->coordinates();
	return Object::isReal() && ((start.x != end.x) || (start.y != end.y));
}

const std::list<ObjectPtr> &LineByTwoPoints::dependsOn() const {
	return myDependsOn;
}

static const std::string TYPE_VALUE = "LineByTwoPoints";
static const std::string KIND_KEY = "kind";
static const std::string START_KEY = "start";
static const std::string END_KEY = "end";

void LineByTwoPoints::saveInfo(ObjectInfoMap &info) const {
	info.addAttribute(TYPE_KEY, TYPE_VALUE);
	info.addAttribute(KIND_KEY, myKind);
	info.addAttribute(START_KEY, myStartPoint);
	info.addAttribute(END_KEY, myEndPoint);
}

LinePtr LineByTwoPoints::create(const ObjectInfoMap &info) {
	LinePtr ptr;

	if (info.stringValueEqualsTo(TYPE_KEY, TYPE_VALUE)) {
		PointPtr startPoint = info.getObjectValue(START_KEY);
		PointPtr endPoint = info.getObjectValue(END_KEY);
		LineKind kind = (LineKind)info.getIntValue(KIND_KEY);
		if (!startPoint.isNull() && !endPoint.isNull()) {
			ptr = new LineByTwoPoints(startPoint, endPoint, kind);
		}
	}

	return ptr;
}

std::list<Polynom> LineByTwoPoints::polynoms(Translator &translator) const {
	return std::list<Polynom>();
}
