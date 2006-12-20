#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include "Object.h"
#include "PointPtr.h"
#include "CirclePtr.h"

class Circle : public Object {
	protected:
		virtual ~Circle() {}

	public:
		static const int RTTI;
		int rtti() const;

		virtual const Coordinates center() const = 0;
		virtual double radius() const = 0;
		virtual PointPtr centerPoint() const;

		double distance(double x, double y) const;
		double distance(const Coordinates coords) const;
		double distance(PointPtr point) const;
};

class CircleByCenterAndPoint : public Circle {
	private:
		const PointPtr myCenter, myPoint;
		std::list<ObjectPtr> myDependsOn;

		CircleByCenterAndPoint(PointPtr center, PointPtr point);

	public:
		static CirclePtr create(PointPtr center, PointPtr point);
		static CirclePtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;

		const Coordinates center() const;
		double radius() const;
		PointPtr centerPoint() const;

		const std::list<ObjectPtr> &dependsOn() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		CircleByCenterAndPoint(const CircleByCenterAndPoint&);
		CircleByCenterAndPoint &operator=(const CircleByCenterAndPoint&);
};

class CircleCopy : public Circle {
	private:
		const PointPtr myCenter;
		const CirclePtr myBase;
		std::list<ObjectPtr> myDependsOn;

		CircleCopy(PointPtr center, CirclePtr base);

	public:
		static CirclePtr create(PointPtr center, CirclePtr base);
		static CirclePtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;

		const Coordinates center() const;
		double radius() const;
		PointPtr centerPoint() const;

		const std::list<ObjectPtr> &dependsOn() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		CircleCopy(const CircleCopy&);
		CircleCopy &operator=(const CircleCopy&);
};

#endif /* __CIRCLE_H__ */
