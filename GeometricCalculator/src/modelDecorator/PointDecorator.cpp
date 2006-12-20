#include "Decorator.h"
#include "../polynom/Polynom.h"

PointDecorator::PointDecorator(PointPtr point) : Point(point->name()), myPoint(point), myCoordinates(point->coordinates()) {
}

const Coordinates PointDecorator::coordinates_internal() const {
	return myCoordinates;
}

void PointDecorator::moveTo_internal(double x, double y) {
	myCoordinates.x = x;
	myCoordinates.y = y;
}

PointPtr PointDecorator::stored() const {
	return myPoint;
}

PointPtr PointDecorator::restore() {
	myPoint->moveTo(myCoordinates);
	return myPoint;
}

std::list<Polynom> PointDecorator::polynoms(Translator &translator) const {
	return std::list<Polynom>();
}
