#ifndef __POINTPTR_H__
#define __POINTPTR_H__

#include "Object.h"

class Point;
class PointPtr;
class PointWeakPtr;

class PointPtr : public ObjectPtr {
	public:
		PointPtr();
		PointPtr(const PointPtr &ptr);
		PointPtr(const PointWeakPtr &ptr);
		PointPtr(const ObjectPtr &ptr);
		PointPtr(Point *point);
		const PointPtr &operator = (const PointPtr &ptr);
		const PointPtr &operator = (const PointWeakPtr &ptr);
		const PointPtr &operator = (const ObjectPtr &ptr);
		const PointPtr &operator = (Point *point);

		Point* operator -> () const;
		Point& operator * () const;
};

class PointWeakPtr : public ObjectWeakPtr {
	public:
		PointWeakPtr();
		PointWeakPtr(const PointPtr &ptr);
		PointWeakPtr(const PointWeakPtr &ptr);
		const PointWeakPtr &operator = (const PointPtr &ptr);
		const PointWeakPtr &operator = (const PointWeakPtr &ptr);

		Point* operator -> () const;
		Point& operator * () const;
};

#endif /* __POINTPTR_H__ */
