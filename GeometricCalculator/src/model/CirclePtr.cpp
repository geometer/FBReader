#include "Circle.h"
#include "CirclePtr.h"

CirclePtr::CirclePtr() : ObjectPtr(NULL) {
}

CirclePtr::CirclePtr(const CirclePtr &ptr) : ObjectPtr(ptr) {
}

CirclePtr::CirclePtr(const ObjectPtr &ptr) {
	operator = (ptr);
}

CirclePtr::CirclePtr(Circle *circle) : ObjectPtr(circle) {
}

const CirclePtr &CirclePtr::operator = (const CirclePtr &ptr) {
	ObjectPtr::operator = (ptr);
	return *this;
}

const CirclePtr &CirclePtr::operator = (const ObjectPtr &ptr) {
	if (!ptr.isNull() && ptr->rtti() == Object::CIRCLE) {
		ObjectPtr::operator = (ptr);
	} else {
		ObjectPtr::operator = (NULL);
	}
	return *this;
}

const CirclePtr &CirclePtr::operator = (Circle *circle) {
	ObjectPtr::operator = (circle);
	return *this;
}

Circle* CirclePtr::operator -> () const {
	return (Circle*)ObjectPtr::operator -> ();
}

Circle& CirclePtr::operator * () const {
	return (Circle&)ObjectPtr::operator * ();
}
