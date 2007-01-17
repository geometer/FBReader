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

#include "Polynom.h"

std::vector<const char*> Monom::ourPredefinedVariableNames;

const char* Monom::getVariableName(const unsigned int number) {
  if (ourPredefinedVariableNames.size() > number) {
    return ourPredefinedVariableNames[number];
  }
	return NULL;
}

int Monom::getVariableNumber(const char *name) {
  for (unsigned int number = 0; number < ourPredefinedVariableNames.size(); number++) {
    if (strcmp(name, ourPredefinedVariableNames[number]) == 0) {
      return number;
    }
  }
  return -1;
}

Monom::Monom(const char *name) : myCoefficient(1) {
  int number = getVariableNumber(name);
  if (number == -1) {
    number = ourPredefinedVariableNames.size();
		char *storedName = new char[strlen(name) + 1];
		strcpy(storedName, name);
    ourPredefinedVariableNames.push_back(storedName);
  }

  myDegree.push_back(DegreePair(number, 1));
}

/*
Monom::Monom(const char *name) : myCoefficient(1), myTotalDegree(1) {
  int number = getVariableNumber(name);
  if (number == -1) {
    number = ourPredefinedVariableNames.size();
    addVariableName(name);
  }

  myDegree.push_back(DegreePair(number, 1));
}
*/
