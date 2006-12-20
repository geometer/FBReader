#include "Point.h"
#include "Circle.h"
#include "../polynom/Polynom.h"
#include "../translator/Translator.h"

#include <math.h>

CirclesCrossing::CirclesCrossing(CirclePtr circle0, CirclePtr circle1, bool first, const std::string &name) : Point(name), myCircle0(circle0), myCircle1(circle1), myFirst(first) {
	myDependsOn.push_back(myCircle0);
	myDependsOn.push_back(myCircle1);
}

PointPtr CirclesCrossing::create(CirclePtr circle0, CirclePtr circle1, bool first, const std::string &name) {
	return new CirclesCrossing(circle0, circle1, first, name);
}

const Coordinates CirclesCrossing::coordinates_internal() const {
	if (!myCircle0->isReal() || !myCircle1->isReal()) {
		return Coordinates(0, 0, false);
	}

	// (x - x0) ^ 2 + (y - y0) ^ 2 = R0 ^ 2
	// (x - x1) ^ 2 + (y - y1) ^ 2 = R1 ^ 2
  // 2 (x0 - x1) x + 2 (y0 - y1) y + x1 ^ 2 - x0 ^ 2 + y1 ^ 2 - y0 ^ 2 + R0 ^ 2 - R1 ^ 2

	const Coordinates center0 = myCircle0->center();
	double x0 = center0.x;
	double y0 = center0.y;
	double R0 = myCircle0->radius();
	const Coordinates center1 = myCircle1->center();
	double x1 = center1.x;
	double y1 = center1.y;
	double R1 = myCircle1->radius();

	double dx = x1 - x0;
	double dy = y1 - y0;
	double distance = sqrt(dx * dx + dy * dy);
	if ((distance == 0) || (distance < fabs(R0 - R1) - 1e-4) || (distance > R0 + R1 + 1e-4)) {
		return Coordinates(0, 0, false);
	}

	double D = x1 * x1 - x0 * x0 + y1 * y1 - y0 * y0 + R0 * R0 - R1 * R1;

	if (fabs(dy) > fabs(dx)) {
		// y = alpha x + beta
		double alpha = - dx / dy;
		double beta = D / dy / 2;
		// (x - x0) ^ 2 + (alpha x + beta - y0) ^ 2 = R0 ^ 2
		// (1 + alpha ^ 2) x ^ 2 + 2 (alpha * (beta - y0) - x0) x + x0 ^ 2 + (beta - y0) ^ 2 - R0 ^ 2 = 0
		double A = 1 + alpha * alpha;
		double B = alpha * (beta - y0) - x0;
		double C = x0 * x0 + (beta - y0) * (beta - y0) - R0 * R0;
		double discriminant = B * B - A * C;
		double root = 0;
		if (discriminant >= 0) {
			root = (myFirst == (y0 > y1)) ? sqrt(discriminant) : -sqrt(discriminant);
		}
		double x = (root - B) / A;
		double y = alpha * x + beta;
		return Coordinates(x, y, true);
	} else {
		double alpha = - dy / dx;
		double beta = D / dx / 2;
		double A = 1 + alpha * alpha;
		double B = alpha * (beta - x0) - y0;
		double C = y0 * y0 + (beta - x0) * (beta - x0) - R0 * R0;
		double discriminant = B * B - A * C;
		double root = 0;
		if (discriminant >= 0) {
			root = (myFirst == (x0 < x1)) ? sqrt(discriminant) : -sqrt(discriminant);
		}
		double y = (root - B) / A;
		double x = alpha * y + beta;
		return Coordinates(x, y, true);
	}
}

const std::list<ObjectPtr> &CirclesCrossing::dependsOn() const {
	return myDependsOn;
}

static const std::string TYPE_VALUE = "CirclesCrossing";
static const std::string CIRCLE0_KEY = "circle0";
static const std::string CIRCLE1_KEY = "circle1";
static const std::string FIRST_KEY = "first";

void CirclesCrossing::saveInfo(ObjectInfoMap &info) const {
	info.addAttribute(TYPE_KEY, TYPE_VALUE);
	info.addAttribute(NAME_KEY, name());
	info.addAttribute(CIRCLE0_KEY, myCircle0);
	info.addAttribute(CIRCLE1_KEY, myCircle1);
	info.addAttribute(FIRST_KEY, myFirst);
}

PointPtr CirclesCrossing::create(const ObjectInfoMap &info) {
	PointPtr ptr;

	if (info.stringValueEqualsTo(TYPE_KEY, TYPE_VALUE)) {
		const std::string &name = info.getStringValue(NAME_KEY);
		CirclePtr circle0 = info.getObjectValue(CIRCLE0_KEY);
		CirclePtr circle1 = info.getObjectValue(CIRCLE1_KEY);
		bool first = info.getBooleanValue(FIRST_KEY);
		if (!circle0.isNull() && !circle1.isNull()) {
			ptr = new CirclesCrossing(circle0, circle1, first, name);
		}
	}

	return ptr;
}

std::list<Polynom> CirclesCrossing::polynoms(Translator &translator) const {
	// TODO: implement
	return std::list<Polynom>();
}
