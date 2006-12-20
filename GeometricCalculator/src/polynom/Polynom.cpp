#include "Polynom.h"

static Monom ZERO = 0;

const Monom& Polynom::getLeadingMonom() const {
  if (myMonoms.empty()) {
    return ZERO;
  } else {
    return myMonoms.front();
  }
}
