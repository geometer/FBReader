#ifndef __POINT_H__
#define __POINT_H__

#include <list>
#include <string>
#include <map>

#include "Object.h"
#include "PointPtr.h"
#include "LinePtr.h"
#include "CirclePtr.h"

class Point : public Object {
	private:
		static std::map<const Point*const,Coordinates> ourCoordinatesCache;

	private:
		std::string myName;

	protected:
		virtual ~Point();

		virtual void moveTo_internal(double x, double y);
		virtual const Coordinates coordinates_internal() const = 0;

	public:
		static const std::string NAME_KEY;

		Point(const std::string &name = "");

		static const int RTTI;
		int rtti() const;

		void setName(const std::string &name);
		const std::string &name() const;

		virtual bool canMove() const;
		void moveTo(double x, double y);
		void moveTo(const Coordinates coords);
		const Coordinates coordinates() const;
		bool isReal() const;

		double distance(double x, double y) const;
		double distance(const Coordinates coords) const;
		double distance(PointPtr point) const;

	private:
		Point(const Point&);
		Point &operator=(const Point&);
};

class IndependentPoint : public Point {
	private:
		Coordinates myCoordinates;

		IndependentPoint(int x, int y, const std::string &name);

	protected:
		void moveTo_internal(double x, double y);
		const Coordinates coordinates_internal() const;

	public:
		static PointPtr create(int x, int y, const std::string &name = "");
		static PointPtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;

		bool canMove() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		IndependentPoint(const IndependentPoint&);
		IndependentPoint &operator=(const IndependentPoint&);
};

class FourthParallelogramPoint : public Point {
	private:
		const PointPtr myFirst, mySecond, myThird;
		std::list<ObjectPtr> myDependsOn;

		FourthParallelogramPoint(PointPtr first, PointPtr second, PointPtr third, const std::string &name);

	protected:
		const Coordinates coordinates_internal() const;

	public:
		static PointPtr create(PointPtr first, PointPtr second, PointPtr third, const std::string &name = "");
		static PointPtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;

		const std::list<ObjectPtr> &dependsOn() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		FourthParallelogramPoint(const FourthParallelogramPoint&);
		FourthParallelogramPoint &operator=(const FourthParallelogramPoint&);
};

class PointOnTheLine : public Point {
	private:
		const PointPtr myPoint0, myPoint1;
		double myAlpha;
		bool myIsFixed;
		PointWeakPtr myWeakPtr;
		std::list<ObjectPtr> myDependsOn;
		LineWeakPtr myLine;

		PointOnTheLine(PointPtr point0, PointPtr point1, LinePtr line, double alpha, const std::string &name);
		~PointOnTheLine();

	protected:
		void moveTo_internal(double x, double y);
		const Coordinates coordinates_internal() const;

	public:
		static PointPtr create(PointPtr point0, PointPtr point1, LinePtr line, double alpha, const std::string &name = "");
		static PointPtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;

		bool canMove() const;

		bool isFixed() const;
		void setFixed(bool fixed);

		const std::list<ObjectPtr> &dependsOn() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		PointOnTheLine(const PointOnTheLine&);
		PointOnTheLine &operator=(const PointOnTheLine&);
};

class PointOnTheCircle : public Point {
	private:
		const CirclePtr myCircle;
		double myAngle;
		std::list<ObjectPtr> myDependsOn;

		PointOnTheCircle(CirclePtr circle, double angle, const std::string &name);

	protected:
		void moveTo_internal(double x, double y);
		const Coordinates coordinates_internal() const;

	public:
		static PointPtr create(CirclePtr circle, double angle, const std::string &name = "");
		static PointPtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;

		bool canMove() const;

		const std::list<ObjectPtr> &dependsOn() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		PointOnTheCircle(const PointOnTheCircle&);
		PointOnTheCircle &operator=(const PointOnTheCircle&);
};

class PointOnPerpendicular : public Point {
	private:
		const LinePtr myLine;
		const PointPtr myPoint;
		std::list<ObjectPtr> myDependsOn;

		PointOnPerpendicular(LinePtr line, PointPtr point, const std::string &name);

	protected:
		const Coordinates coordinates_internal() const;

	public:
		static PointPtr create(LinePtr line, PointPtr point, const std::string &name = "");
		static PointPtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;

		const std::list<ObjectPtr> &dependsOn() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		PointOnPerpendicular(const PointOnPerpendicular&);
		PointOnPerpendicular &operator=(const PointOnPerpendicular&);
};

class LinesCrossing : public Point {
	private:
		const LinePtr myLine0, myLine1;
		std::list<ObjectPtr> myDependsOn;
		PointWeakPtr myWeakPtr;

		LinesCrossing(LinePtr line1, LinePtr line2, const std::string &name);
		~LinesCrossing();

	protected:
		const Coordinates coordinates_internal() const;

	public:
		static PointPtr create(LinePtr line1, LinePtr line2, const std::string &name = "");
		static PointPtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;

		const std::list<ObjectPtr> &dependsOn() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		LinesCrossing(const LinesCrossing&);
		LinesCrossing &operator=(const LinesCrossing&);
};

class LineAndCircleCrossing : public Point {
	private:
		// TODO: revert const
		const LinePtr myLine;
		const CirclePtr myCircle;
		bool myFirst;
		std::list<ObjectPtr> myDependsOn;
		PointWeakPtr myWeakPtr;

		LineAndCircleCrossing(LinePtr line, CirclePtr circle, bool first, const std::string &name);
		~LineAndCircleCrossing();

	protected:
		const Coordinates coordinates_internal() const;

	public:
		static PointPtr create(LinePtr line, CirclePtr circle, bool first, const std::string &name = "");
		static PointPtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;

		const std::list<ObjectPtr> &dependsOn() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		LineAndCircleCrossing(const LineAndCircleCrossing&);
		LineAndCircleCrossing &operator=(const LineAndCircleCrossing&);
};

class CirclesCrossing : public Point {
	private:
		const CirclePtr myCircle0, myCircle1;
		bool myFirst;
		std::list<ObjectPtr> myDependsOn;

		CirclesCrossing(CirclePtr line1, CirclePtr line2, bool first, const std::string &name);

	protected:
		const Coordinates coordinates_internal() const;

	public:
		static PointPtr create(CirclePtr line1, CirclePtr line2, bool first, const std::string &name = "");
		static PointPtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;


		const std::list<ObjectPtr> &dependsOn() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		CirclesCrossing(const CirclesCrossing&);
		CirclesCrossing &operator=(const CirclesCrossing&);
};

#endif /* __POINT_H__ */
