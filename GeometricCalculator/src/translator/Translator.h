#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <map>

#include "../model/Object.h"
#include "../polynom/Polynom.h"

class Translator {
	private:
		std::map<ObjectPtr,int> myObjects;

		Monom monom(int objectNumber, const char *name);
		
	public:
		Translator(const std::list<ObjectPtr> &objects);
		Monom monom(const Object &object, const char *name);
		Monom monom(ObjectPtr object, const char *name);
		std::list<Polynom> translate();
};

#endif /* __TRANSLATOR_H__ */
