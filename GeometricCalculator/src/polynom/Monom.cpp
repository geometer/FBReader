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
