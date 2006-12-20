#ifndef __LINEPTR_H__
#define __LINEPTR_H__

#include "Object.h"

class Line;
class LinePtr;
class LineWeakPtr;

class LinePtr : public ObjectPtr {
	public:
		LinePtr();
		LinePtr(const LinePtr &ptr);
		LinePtr(const LineWeakPtr &ptr);
		LinePtr(const ObjectPtr &ptr);
		LinePtr(Line *line);
		const LinePtr &operator = (const LinePtr &ptr);
		const LinePtr &operator = (const LineWeakPtr &ptr);
		const LinePtr &operator = (const ObjectPtr &ptr);
		const LinePtr &operator = (Line *line);

		Line* operator -> () const;
		Line& operator * () const;
};

class LineWeakPtr : public ObjectWeakPtr {
	public:
		LineWeakPtr();
		LineWeakPtr(const LinePtr &ptr);
		LineWeakPtr(const LineWeakPtr &ptr);
		const LineWeakPtr &operator = (const LinePtr &ptr);
		const LineWeakPtr &operator = (const LineWeakPtr &ptr);

		Line* operator -> () const;
		Line& operator * () const;
};

#endif /* __LINEPTR_H__ */
