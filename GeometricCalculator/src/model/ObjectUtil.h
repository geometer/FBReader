#ifndef __OBJECTUTIL_H__
#define __OBJECTUTIL_H__

#include "Object.h"

class ObjectUtil {
	public:
		static std::list<ObjectPtr> orderedClosure(const std::list<ObjectPtr> &objects);
};

#endif /* __OBJECTUTIL_H__ */
