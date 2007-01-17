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

#ifndef __DECORATOR_H__
#define __DECORATOR_H__

#include <shared_ptr.h>

#include "../model/Object.h"
#include "../model/Point.h"
#include "../model/Scene.h"

class PointDecorator : public Point {
	private:
		PointPtr myPoint;
		Coordinates myCoordinates;

	protected:
		const Coordinates coordinates_internal() const;
		void moveTo_internal(double x, double y);

	public:
		PointDecorator(PointPtr point);

		PointPtr stored() const;
		PointPtr restore();

		std::list<Polynom> polynoms(Translator &translator) const;
		void saveInfo(ObjectInfoMap &info) const {}
};

class SceneDecorator : public Scene {
  public:	
		SceneDecorator(shared_ptr<Scene> &scene);
		shared_ptr<Scene> restore();

		void add(ObjectPtr object);
		void remove(ObjectPtr object);

		std::list<ObjectPtr>::iterator find(ObjectPtr object);
		std::list<ObjectPtr>::const_iterator const_find(ObjectPtr object) const;
};

#endif /* __DECORATOR_H__ */
