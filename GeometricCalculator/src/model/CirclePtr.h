#ifndef __CIRCLEPTR_H__
#define __CIRCLEPTR_H__

#include "Object.h"

class Circle;

class CirclePtr : public ObjectPtr {
	public:
		CirclePtr();
		CirclePtr(const CirclePtr &ptr);
		CirclePtr(const ObjectPtr &ptr);
		CirclePtr(Circle *circle);
		const CirclePtr &operator = (const CirclePtr &ptr);
		const CirclePtr &operator = (const ObjectPtr &ptr);
		const CirclePtr &operator = (Circle *circle);

		Circle* operator -> () const;
		Circle& operator * () const;
};

#endif /* __CIRCLEPTR_H__ */
