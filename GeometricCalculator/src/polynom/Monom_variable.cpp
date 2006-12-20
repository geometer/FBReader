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
