#include "Point.h"

PointWeakPtr::PointWeakPtr() : ObjectWeakPtr() {
}

PointWeakPtr::PointWeakPtr(const PointPtr &ptr) : ObjectWeakPtr(ptr) {
}

PointWeakPtr::PointWeakPtr(const PointWeakPtr &ptr) : ObjectWeakPtr(ptr) {
}

const PointWeakPtr &PointWeakPtr::operator = (const PointPtr &ptr) {
	ObjectWeakPtr::operator = (ptr);
	return *this;
}

const PointWeakPtr &PointWeakPtr::operator = (const PointWeakPtr &ptr) {
	ObjectWeakPtr::operator = (ptr);
	return *this;
}

Point* PointWeakPtr::operator -> () const {
	return (Point*)ObjectWeakPtr::operator -> ();
}

Point& PointWeakPtr::operator * () const {
	return (Point&)ObjectWeakPtr::operator * ();
}
