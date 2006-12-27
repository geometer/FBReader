#ifndef __LINE_H__
#define __LINE_H__

#include "Object.h"
#include "PointPtr.h"
#include "LinePtr.h"

enum LineKind {
	LINE,
	HALFLINE,
	SEGMENT
};

enum LineEnd {
	START,
	END
};

class Line : public Object {
	protected:
		virtual ~Line() {}

	public:
		Type rtti() const;

		virtual const PointPtr point(LineEnd end) const = 0;
		virtual const std::list<PointWeakPtr> &points() const;
		virtual void connectPoint(const PointPtr point) = 0;
		virtual void disconnectPoint(const PointWeakPtr point) = 0;
		bool contains(const PointPtr point) const;
		virtual LineKind kind() const = 0;

		double distance(double x, double y) const;
		double distance(const Coordinates coords) const;
		double distance(const PointPtr point) const;
};

class LineByTwoPoints : public Line {
	private:
		const PointPtr myStartPoint, myEndPoint;
		std::list<PointWeakPtr> myPoints;
		LineKind myKind;
		std::list<ObjectPtr> myDependsOn;

		LineByTwoPoints(const PointPtr startPoint, const PointPtr endPoint, LineKind kind);

	public:
		static LinePtr create(const PointPtr startPoint, const PointPtr endPoint, LineKind kind);
		static LinePtr create(const ObjectInfoMap &info);
		void saveInfo(ObjectInfoMap &info) const;

		const PointPtr point(LineEnd end) const;
		const std::list<PointWeakPtr> &points() const;
		void connectPoint(const PointPtr point);
		void disconnectPoint(const PointWeakPtr point);
		LineKind kind() const;

		bool isReal() const;

		const std::list<ObjectPtr> &dependsOn() const;

		std::list<Polynom> polynoms(Translator &translator) const;

	private:
		LineByTwoPoints(const LineByTwoPoints&);
		LineByTwoPoints &operator=(const LineByTwoPoints&);
};

#endif /* __LINE_H__ */
