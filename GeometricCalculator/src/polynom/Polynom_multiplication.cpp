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
