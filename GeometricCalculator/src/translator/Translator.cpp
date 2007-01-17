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
