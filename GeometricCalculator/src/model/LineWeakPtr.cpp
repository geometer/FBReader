#include "Line.h"

LineWeakPtr::LineWeakPtr() : ObjectWeakPtr() {
}

LineWeakPtr::LineWeakPtr(const LinePtr &ptr) : ObjectWeakPtr(ptr) {
}

LineWeakPtr::LineWeakPtr(const LineWeakPtr &ptr) : ObjectWeakPtr(ptr) {
}

const LineWeakPtr &LineWeakPtr::operator = (const LinePtr &ptr) {
	ObjectWeakPtr::operator = (ptr);
	return *this;
}

const LineWeakPtr &LineWeakPtr::operator = (const LineWeakPtr &ptr) {
	ObjectWeakPtr::operator = (ptr);
	return *this;
}

Line* LineWeakPtr::operator -> () const {
	return (Line*)ObjectWeakPtr::operator -> ();
}

Line& LineWeakPtr::operator * () const {
	return (Line&)ObjectWeakPtr::operator * ();
}
