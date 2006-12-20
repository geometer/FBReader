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
