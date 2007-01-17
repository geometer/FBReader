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
