#include "Line.h"

LinePtr::LinePtr() : ObjectPtr(NULL) {
}

LinePtr::LinePtr(const LinePtr &ptr) : ObjectPtr(ptr) {
}

LinePtr::LinePtr(const LineWeakPtr &ptr) : ObjectPtr(ptr) {
}

LinePtr::LinePtr(const ObjectPtr &ptr) {
	operator = (ptr);
}

LinePtr::LinePtr(Line *line) : ObjectPtr(line) {
}

const LinePtr &LinePtr::operator = (const LinePtr &ptr) {
	ObjectPtr::operator = (ptr);
	return *this;
}

const LinePtr &LinePtr::operator = (const LineWeakPtr &ptr) {
	ObjectPtr::operator = (ptr);
	return *this;
}

const LinePtr &LinePtr::operator = (const ObjectPtr &ptr) {
	if (!ptr.isNull() && ptr->rtti() == Line::RTTI) {
		ObjectPtr::operator = (ptr);
	} else {
		ObjectPtr::operator = (NULL);
	}
	return *this;
}

const LinePtr &LinePtr::operator = (Line *line) {
	ObjectPtr::operator = (line);
	return *this;
}

Line* LinePtr::operator -> () const {
	return (Line*)ObjectPtr::operator -> ();
}

Line& LinePtr::operator * () const {
	return (Line&)ObjectPtr::operator * ();
}
