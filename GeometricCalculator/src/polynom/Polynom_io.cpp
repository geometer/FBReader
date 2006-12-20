#include "Polynom_io.h"

std::ostream& operator << (std::ostream &os, const Number &number) {
	unsigned int len = number.strlen();
	char *buffer = new char[len + 1];
	os << number.sprintf(buffer);
	delete[] buffer;
	return os;
}

std::ostream& write (std::ostream &os, const Monom &monom, bool withSign) {
  //if (monom.getCoefficient().isPositive()) {
  if (monom.getCoefficient() > 0) {
    if (withSign) {
      os << "+ ";
    }
  } else {
    os << "- ";
  }

  int maxVariableIndex = monom.getLeadingVariableIndex();
  
  if ((maxVariableIndex == -1) ||
      (abs(monom.getCoefficient()) != 1)) {
    os << abs(monom.getCoefficient()) << " ";
  }

  for (int i = 0; i <= maxVariableIndex; i++) {
    unsigned int degree = monom.getDegree(i);
    if (degree) {
      os << Monom::getVariableName(i); 
      if (degree > 1) {
        os << "^" << degree;
      }
      os << " ";
    }
  }
  return os;
}

std::ostream& operator << (std::ostream &os, const Monom &monom) {
  return write(os, monom, false);
}

std::ostream& operator << (std::ostream &os, const Polynom &polynom) {
  if (polynom.isZero()) {
    os << 0;
  } else {
		std::list<Monom>::const_iterator monom_iterator;
    unsigned int monom_counter;

    for (monom_iterator = polynom.monoms().begin(), monom_counter = 0;
         monom_iterator != polynom.monoms().end();
         monom_iterator++, monom_counter++) {
      write(os, *monom_iterator, monom_counter > 0);
    }
  }

  return os;
}
