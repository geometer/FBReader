/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include "Object.h"
#include "PointPtr.h"
#include "CirclePtr.h"

class Circle : public Object {
	protected:
		virtual ~Circle() {}

	public:
		Type rtti() const;

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
