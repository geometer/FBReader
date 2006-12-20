#include "Polynom.h"

Polynom& Polynom::operator += (const Monom &monom) {
  if (monom.isZero()) {
    return *this;
  }

	std::list<Monom>::iterator it;
  for (it = myMonoms.begin(); it != myMonoms.end(); it++) {
    if (monom >= *it) {
      if ((*it).isProportional(monom)) {
        (*it).myCoefficient += monom.myCoefficient;
	if ((*it).myCoefficient == 0) {
	  myMonoms.erase(it);
	}
	return *this;
      }
      break;
    }
  }
  myMonoms.insert(it, monom);
  return *this;
}

Polynom& Polynom::operator += (const Polynom &polynom) {
	std::list<Monom>::iterator it = myMonoms.begin();
	std::list<Monom>::const_iterator jt = polynom.myMonoms.begin();
  while ((it != myMonoms.end()) && (jt != polynom.myMonoms.end())) {
    if (*jt < *it) {
      it++;
    } else if ((*it).isProportional(*jt)) {
      (*it).myCoefficient += (*jt).myCoefficient;
      if ((*it).myCoefficient == 0) {
        it = myMonoms.erase(it);
      } else {
        it++;
      }
      jt++;
    } else {
      myMonoms.insert(it, *jt);
      jt++;
    }
  }
  myMonoms.insert(myMonoms.end(), jt, polynom.myMonoms.end());

  return *this;
}

Polynom& Polynom::add (Polynom &polynom) {
	std::list<Monom>::iterator it = myMonoms.begin();
  std::list<Monom>::iterator jt = polynom.myMonoms.begin(), kt;
  while ((it != myMonoms.end()) && (jt != polynom.myMonoms.end())) {
    if (*jt < *it) {
      it++;
    } else if ((*it).isProportional(*jt)) {
      (*it).myCoefficient += (*jt).myCoefficient;
      //(*it).myCoefficient.add((*jt).myCoefficient);
      if ((*it).myCoefficient == 0) {
        it = myMonoms.erase(it);
      } else {
        it++;
      }
      jt++;
    } else {
      kt = jt;
      jt++;
      myMonoms.splice(it, polynom.myMonoms, kt);
    }
  }
  myMonoms.splice(myMonoms.end(), polynom.myMonoms, jt, polynom.myMonoms.end());

  return *this;
}
