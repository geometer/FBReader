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

Monom& Monom::operator *= (const Monom &monom) {
  myCoefficient *= monom.myCoefficient;
  
  if (!monom.myDegree.empty()) {
    DegreeList::iterator it;
    DegreeList::const_iterator jt;
    for (it = myDegree.begin(), jt = monom.myDegree.begin();
         (it != myDegree.end()) && (jt != monom.myDegree.end());) {
      if ((*it).first > (*jt).first) {
        it++;
      } else if ((*it).first < (*jt).first) {
        myDegree.insert(it, *jt);
        jt++;
      } else {
        (*it).second += (*jt).second;
        it++;
        jt++;
      }
    }
    for (; jt != monom.myDegree.end(); jt++) {
      myDegree.push_back(*jt);
    }
  }
  return *this;
}

Polynom& Polynom::operator *= (int number) {
  if (number == 0) {
    myMonoms.clear();
  } else if (number != 1) {
    for (std::list<Monom>::iterator it = myMonoms.begin(); it != myMonoms.end(); it++) {
      *it *= number;
    }
  }
  return *this;
}

Polynom& Polynom::operator *= (const Number &number) {
  if (number == 0) {
    myMonoms.clear();
  } else if (number != 1) {
    for (std::list<Monom>::iterator it = myMonoms.begin(); it != myMonoms.end(); it++) {
      *it *= number;
    }
  }
  return *this;
}

Polynom& Polynom::operator *= (const Monom &monom) {
  if (monom.isZero()) {
    myMonoms.clear();
  } else if (monom != 1) {
    for (std::list<Monom>::iterator it = myMonoms.begin(); it != myMonoms.end(); it++) {
      *it *= monom;
    }
  }
  return *this;
}

Polynom Polynom::operator * (const Polynom &polynom) const {
  Polynom result;
  if (!polynom.isZero()) {
		std::list<Monom>::const_iterator it;
    for (it = polynom.myMonoms.begin(); it != polynom.myMonoms.end(); it++) {
      //result += (*this) * (*it);
      Polynom summand = (*this) * (*it);
      result.add(summand);
    }
  }
  return result;
}

Polynom Polynom::operator ^ (const unsigned int power) const {
  if (power) {
    Polynom result = *this;
    for (unsigned int i = 1; i < power; i++) {
      result *= *this;
    }
    return result;
  } else {
    return 1;
  }
}
