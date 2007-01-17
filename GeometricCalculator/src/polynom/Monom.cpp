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

/*
bool Monom::divides(const Monom &other) const {
  DegreeList::const_iterator it, jt;
  for (it = myDegree.begin(), jt = other.myDegree.begin();
       (it != myDegree.end()) && (jt != other.myDegree.end());) {
    if ((*it).first > (*jt).first) {
      return false;
    }
    if ((*it).first < (*jt).first) {
      jt++;
    } else {
      if ((*it).second > (*jt). second) {
        return false;
      }
      it++;
      jt++;
    }
  }
  return (it == myDegree.end());
}

bool Monom::relativelyPrime(const Monom &other) const {
  DegreeList::const_iterator it, jt;
  for (it = myDegree.begin(), jt = other.myDegree.begin();
       (it != myDegree.end()) && (jt != other.myDegree.end());) {
    if ((*it).first == (*jt).first) {
      return false;
    }
    if ((*it).first > (*jt).first) {
      it++;
    } else {
      jt++;
    }
  }
  return true;
}
*/

unsigned int Monom::getDegree(const unsigned int variableNumber) const {
  DegreeList::const_iterator it;
  for (it = myDegree.begin(); it != myDegree.end(); it++) {
    if ((*it).first == variableNumber) {
      return (*it).second;
    }
    if ((*it).first < variableNumber) {
      break;
    }
  }
  return 0;
}
