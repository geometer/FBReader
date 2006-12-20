#include "Translator.h"
#include "../model/ObjectUtil.h"
#include "../model/Point.h"

#include <stdio.h>

Translator::Translator(const std::list<ObjectPtr> &objects) {
	std::list<ObjectPtr> closure = ObjectUtil::orderedClosure(objects);
	int counter = 0;
	for (std::list<ObjectPtr>::iterator it = closure.begin(); it != closure.end(); it++) {
		myObjects[*it] = counter++;
	}
}

std::list<Polynom> Translator::translate() {
	std::list<Polynom> polynoms;

	for (std::map<ObjectPtr,int>::const_iterator it = myObjects.begin(); !(it == myObjects.end()); it++) {
		std::list<Polynom> pointPolynoms = (*it).first->polynoms(*this);
		polynoms.insert(polynoms.end(), pointPolynoms.begin(), pointPolynoms.end());
	}

	return polynoms;
}

Monom Translator::monom(const Object &object, const char *name) {
	for (std::map<ObjectPtr,int>::const_iterator it = myObjects.begin(); !(it == myObjects.end()); it++) {
		ObjectPtr ptr = (*it).first;
		if (ptr.operator->() == &object) {
			return monom((*it).second, name);
		}
	}
	return -1;
}

Monom Translator::monom(ObjectPtr object, const char *name) {
	return monom(myObjects[object], name);
}

Monom Translator::monom(int objectNumber, const char *name) {
	int length = strlen(name) + 3;
  for (int n = objectNumber / 10; n > 0; n /= 10) {
		length++;
	}
	char *fullName = new char[length];
	
	sprintf(fullName, "%s_%d", name, objectNumber);
	Monom monom(fullName);
	delete[] fullName;
	
	return monom;
}
