#include "Value.h"

Distance::Distance(PointPtr point0, PointPtr point1) : myPoint0(point0), myPoint1(point1) {
	myDependsOn.push_back(myPoint0);
	myDependsOn.push_back(myPoint1);
}

PointPtr Distance::startPoint() const {
	return myPoint0;
}

PointPtr Distance::endPoint() const {
	return myPoint1;
}

const std::list<ObjectPtr> &Distance::dependsOn() const {
	return myDependsOn;
}
