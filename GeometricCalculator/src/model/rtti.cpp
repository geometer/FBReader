#include "Point.h"
#include "Line.h"
#include "Circle.h"

const int Point::RTTI = 1;
const int Line::RTTI = 2;
const int Circle::RTTI = 3;

int Point::rtti() const {
	return RTTI;
}

int Line::rtti() const {
	return RTTI;
}

int Circle::rtti() const {
	return RTTI;
}
