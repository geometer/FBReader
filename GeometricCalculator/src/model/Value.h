#ifndef __VALUE_H__
#define __VALUE_H__

#include "Object.h"
#include "Point.h"

class Value;
typedef shared_ptr<Value> ValuePtr;
typedef weak_ptr<Value> ValueWeakPtr;

class Value {
	public:
		virtual ~Value() {}

		virtual const std::list<ObjectPtr> &dependsOn() const = 0;
};

class Distance : public Value {
	private:
		PointPtr myPoint0, myPoint1;
		std::list<ObjectPtr> myDependsOn;

	public:
		Distance(PointPtr point0, PointPtr point1);
		PointPtr startPoint() const;
		PointPtr endPoint() const;
		const std::list<ObjectPtr> &dependsOn() const;
};

#endif /* __VALUE_H__ */
