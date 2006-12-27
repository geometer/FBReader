#include "Point.h"

PointPtr::PointPtr() {
}

PointPtr::PointPtr(const PointPtr &ptr) : ObjectPtr(ptr) {
}

PointPtr::PointPtr(const PointWeakPtr &ptr) : ObjectPtr(ptr) {
}

PointPtr::PointPtr(const ObjectPtr &ptr) {
	operator = (ptr);
}

PointPtr::PointPtr(Point *point) : ObjectPtr(point) {
}

const PointPtr &PointPtr::operator = (const PointPtr &ptr) {
	ObjectPtr::operator = (ptr);
	return *this;
}

const PointPtr &PointPtr::operator = (const PointWeakPtr &ptr) {
	ObjectPtr::operator = (ptr);
	return *this;
}

const PointPtr &PointPtr::operator = (const ObjectPtr &ptr) {
	if (!ptr.isNull() && ptr->rtti() == Object::POINT) {
		ObjectPtr::operator = (ptr);
	} else {
		ObjectPtr::operator = (0);
	}
	return *this;
}

const PointPtr &PointPtr::operator = (Point *point) {
	ObjectPtr::operator = (point);
	return *this;
}

Point* PointPtr::operator -> () const {
	return (Point*)ObjectPtr::operator -> ();
}

Point& PointPtr::operator * () const {
	return (Point&)ObjectPtr::operator * ();
}
