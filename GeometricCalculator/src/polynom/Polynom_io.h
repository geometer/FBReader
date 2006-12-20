#ifndef __POLYNOM_IO_H__
#define __POLYNOM_IO_H__

#include "Polynom.h"
#include <iostream>

std::ostream& operator << (std::ostream &os, const Number &number);
std::ostream& write (std::ostream &os, const Monom &monom, bool withSign);
std::ostream& operator << (std::ostream &os, const Monom &monom);
std::ostream& operator << (std::ostream &os, const Polynom &polynom);

#endif /* __POLYNOM_IO_H__ */
