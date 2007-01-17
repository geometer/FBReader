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

#ifndef __POLYNOM_H__
#define __POLYNOM_H__

#include "Number.h"

#include <list>
#include <vector>

class Polynom;
class Monom;

Polynom operator - (Polynom polynom);

//Monom lcm(const Monom &monom1, const Monom &monom2);

class Monom {
	friend class Polynom;

  public:
    static const char* getVariableName(const unsigned int index);

  private:
    static int getVariableNumber(const char *name);
    //static void addVariableName(const char *name);
    static std::vector<const char*> ourPredefinedVariableNames;
// 

  private:
    typedef std::pair<unsigned int, unsigned int> DegreePair;
    typedef std::list<DegreePair> DegreeList;

   	Number myCoefficient;
    DegreeList myDegree;
//   unsigned int myTotalDegree;

  public:
    Monom(const Monom &monom) { myCoefficient = monom.myCoefficient; myDegree = monom.myDegree; }
    Monom(const Number &number) { myCoefficient = number; }
    Monom(const int number = 1) { myCoefficient = number; }
    Monom(const char *name);

    const Monom& operator = (const Monom &monom) { myCoefficient = monom.myCoefficient; myDegree = monom.myDegree; return *this; }
    const Monom& operator = (const Number &number) { myCoefficient = number; myDegree.clear(); return *this; }
    const Monom& operator = (const int number) { myCoefficient = number; myDegree.clear(); return *this; }

    bool operator == (const Monom &monom) const { return isProportional(monom) && (myCoefficient == monom.myCoefficient); } ;
    bool operator == (const Number &number) const { return myDegree.empty() && (myCoefficient == number); }
    bool operator == (int number) const { return myDegree.empty() && (myCoefficient == number); }

		bool operator < (const Monom &monom) const { return myDegree < monom.myDegree; }

    Monom& operator *= (const Monom &monom);
    Monom& operator *= (const Number &number) { myCoefficient *= number; return *this; }
    Monom& operator *= (int number) { myCoefficient *= number; return *this; }
    Monom& operator /= (const Monom &monom);
    Monom& operator /= (const Number &number) { myCoefficient /= number; return *this; }
    Monom& operator /= (int number) { myCoefficient /= number; return *this; }

    Polynom operator + (const Monom &monom) const;
    Polynom operator + (const Number &number) const;
    Polynom operator + (int number) const;
    Polynom operator - (const Monom &monom) const;
    Polynom operator - (const Number &number) const;
    Polynom operator - (int number) const;

    Monom operator ^ (const unsigned int) const;

    bool isZero() const { return myCoefficient == 0; };

//   Monom(const std::string &name);
//   Monom(const unsigned int variableNumber, const unsigned int degree);
// 
// 
	  bool isProportional(const Monom &monom) const {
			return ((myCoefficient == 0) && (monom.myCoefficient == 0)) ||
	  					(myDegree == monom.myDegree);
		}
//   bool divides(const Monom&) const;
//   bool relativelyPrime(const Monom&) const;
// 
    const Number& getCoefficient() const { return myCoefficient; }
    int getLeadingVariableIndex() const { return myDegree.empty() ? -1 : (int)myDegree.front().first; }

    unsigned int getDegree(const unsigned int index) const;

//   friend Polynom& Polynom::operator += (const Monom&);
//   friend Polynom& Polynom::operator += (const Polynom&);
//   friend Polynom& Polynom::add(Polynom&);
//   friend bool Polynom::reduce(const Polynom&);
//   friend Monom lcm<>(const Monom&, const Monom&);
};

class Polynom {
  private:
		std::list<Monom> myMonoms;

  public:
    Polynom(const Polynom &polynom) { myMonoms = polynom.myMonoms; }
    Polynom(const Monom &monom) { if (!monom.isZero()) myMonoms.push_back(monom); }
    Polynom(const Number &number) { if (number != 0) myMonoms.push_back(number); }
    Polynom(const int number = 0) { if (number != 0) myMonoms.push_back(number); }

    const Polynom& operator = (const Polynom &polynom) { myMonoms = polynom.myMonoms; return *this; }
    const Polynom& operator = (const Monom &monom) { myMonoms.clear(); if (!monom.isZero()) myMonoms.push_back(monom); return *this; }
    const Polynom& operator = (const Number &number) { myMonoms.clear(); if (number != 0) myMonoms.push_back(number); return *this; }
    const Polynom& operator = (const int number) { myMonoms.clear(); if (number != 0) myMonoms.push_back(number); return *this; }

    bool isZero() const { return myMonoms.empty(); }
//   bool isNonNegative() const;
//   bool isPositive() const;

    bool operator == (const Polynom &polynom) const;
    bool operator == (const Monom &monom) const;
    bool operator == (const Number &number) const;
    bool operator == (int number) const;

//   bool operator < (const Polynom&) const;

    Polynom& operator += (const Polynom &polynom);
    Polynom& operator += (const Monom &monom);
		// TODO: optimize
    Polynom& operator += (const Number &number) { return *this += Monom(number); }
		// TODO: optimize
    Polynom& operator += (int number) { return *this += Monom(number); }
		// TODO: optimize
    Polynom& operator -= (const Polynom &polynom) { return *this += -polynom; }
		// TODO: optimize
    Polynom& operator -= (const Monom &monom) { return *this += -monom; }
		// TODO: optimize
    Polynom& operator -= (const Number &number) { return *this += -number; }
    Polynom& operator -= (int number) { return *this += -number; }
    Polynom& operator *= (const Polynom &polynom) { *this = *this * polynom; return *this; };
    Polynom operator * (const Polynom &polynom) const;
    Polynom& operator *= (const Monom &monom);
    Polynom& operator *= (const Number &number);
    Polynom& operator *= (int number);
    Polynom& operator /= (const Monom &monom);
    Polynom& operator /= (const Number &number);
    Polynom& operator /= (int number);

    Polynom operator ^ (const unsigned int) const;

    /*
     * This function is the same as operator +=,
     * but parameter would be changed.
     *
     * It works faster than operator +=.
     */
    Polynom& add(Polynom &polynom);

//   Polynom operator * (const Polynom&) const;
// 
//   bool reduce(const Polynom&);
//   void normalize();
//   
//   typedef std::list<Monom > MonomList;
    const std::list<Monom>& monoms() const { return myMonoms; }
    const Monom& getLeadingMonom() const;
};

inline bool operator == (const Monom &monom, const Polynom &polynom) { return polynom == monom; }
inline bool operator == (const Number &number, const Polynom &polynom) { return polynom == number; }
inline bool operator == (int number, const Polynom &polynom) { return polynom == number; }
inline bool operator == (const Number &number, const Monom &monom) { return monom == number; }
inline bool operator == (int number, const Monom &monom) { return monom == number; }

inline bool operator != (const Polynom &polynom1, const Polynom &polynom2) { return !(polynom1 == polynom2); }
inline bool operator != (const Polynom &polynom, const Monom &monom) { return !(polynom == monom); }
inline bool operator != (const Monom &monom, const Polynom &polynom) { return !(polynom == monom); }
inline bool operator != (const Polynom &polynom, const Number &number) { return !(polynom == number); }
inline bool operator != (const Number &number, const Polynom &polynom) { return !(polynom == number); }
inline bool operator != (int number, const Polynom &polynom) { return !(polynom == number); }
inline bool operator != (const Polynom &polynom, int number) { return !(polynom == number); }
inline bool operator != (const Monom &monom1, const Monom &monom2) { return !(monom1 == monom2); }
inline bool operator != (const Monom &monom, const Number &number) { return !(monom == number); }
inline bool operator != (const Number &number, const Monom &monom) { return !(monom == number); }
inline bool operator != (const Monom &monom, int number) { return !(monom == number); }
inline bool operator != (int number, const Monom &monom) { return !(monom == number); }

inline bool operator > (const Monom &monom1, const Monom &monom2) { return monom2 < monom1; }
inline bool operator <= (const Monom &monom1, const Monom &monom2) { return !(monom2 < monom1); }
inline bool operator >= (const Monom &monom1, const Monom &monom2) { return !(monom1 < monom2); }

// TODO: optimize
inline Polynom operator - (Polynom polynom) { return polynom *= -1; }
// TODO: optimize
inline Monom operator - (Monom monom) { return monom *= -1; }
inline const Polynom& operator + (const Polynom &polynom) { return polynom; }
inline const Monom& operator + (const Monom &monom) { return monom; }

inline Polynom operator + (Polynom polynom1, const Polynom &polynom2) { return polynom1 += polynom2; }
inline Polynom operator + (Polynom polynom, const Monom &monom) { return polynom += monom; }
inline Polynom operator + (const Monom &monom, Polynom polynom) { return polynom += monom; }
inline Polynom operator + (Polynom polynom, const Number &number) { return polynom += number; }
inline Polynom operator + (const Number &number, Polynom polynom) { return polynom += number; }
inline Polynom operator + (Polynom polynom, int number) { return polynom += number; }
inline Polynom operator + (int number, Polynom polynom) { return polynom += number; }
inline Polynom operator + (const Number &number, const Monom &monom) { return monom + number; }
inline Polynom operator + (int number, const Monom &monom) { return monom + number; }

inline Polynom Monom::operator + (const Monom &monom) const { return Polynom(*this) + monom; }
inline Polynom Monom::operator + (const Number &number) const { return Polynom(*this) + number; }
inline Polynom Monom::operator + (int number) const { return Polynom(*this) + number; }

inline Polynom operator - (Polynom polynom1, const Polynom &polynom2) { return polynom1 -= polynom2; }
inline Polynom operator - (Polynom polynom, const Monom &monom) { return polynom -= monom; }
inline Polynom operator - (const Monom &monom, Polynom polynom) { return -polynom += monom; }
inline Polynom operator - (Polynom polynom, const Number &number) { return polynom -= number; }
inline Polynom operator - (const Number &number, Polynom polynom) { return -polynom += number; }
inline Polynom operator - (Polynom polynom, int number) { return polynom -= number; }
inline Polynom operator - (int number, Polynom polynom) { return -polynom += number; }
inline Polynom operator - (const Number &number, const Monom &monom) { return -monom + number; }
inline Polynom operator - (int number, const Monom &monom) { return -monom + number; }

inline Polynom Monom::operator - (const Monom &monom) const { return Polynom(*this) - monom; }
inline Polynom Monom::operator - (const Number &number) const { return Polynom(*this) - number; }
inline Polynom Monom::operator - (int number) const { return Polynom(*this) - number; }

inline Polynom operator * (Polynom polynom, const Monom &monom) { return polynom *= monom; }
inline Polynom operator * (const Monom &monom, Polynom polynom) { return polynom *= monom; }
inline Polynom operator * (Polynom polynom, const Number &number) { return polynom *= number; }
inline Polynom operator * (const Number &number, Polynom polynom) { return polynom *= number; }
inline Polynom operator * (Polynom polynom, int number) { return polynom *= number; }
inline Polynom operator * (int number, Polynom polynom) { return polynom *= number; }
inline Monom operator * (Monom monom1, const Monom &monom2) { return monom1 *= monom2; }
inline Monom operator * (Monom monom, const Number &number) { return monom *= number; }
inline Monom operator * (const Number &number, Monom monom) { return monom *= number; }
inline Monom operator * (Monom monom, int number) { return monom *= number; }
inline Monom operator * (int number, Monom monom) { return monom *= number; }

inline Polynom operator / (Polynom polynom, const Monom &monom) { return polynom /= monom; }
inline Polynom operator / (Polynom polynom, const Number &number) { return polynom /= number; }
inline Polynom operator / (Polynom polynom, int number) { return polynom /= number; }
inline Monom operator / (Monom monom1, const Monom &monom2) { return monom1 /= monom2; }
inline Monom operator / (Monom monom, const Number &number) { return monom /= number; }
inline Monom operator / (Monom monom, int number) { return monom /= number; }

//  Polynom pseudoRemainder(const Polynom&, const Polynom&);
//  Polynom gcd(const Polynom&, const Polynom&);
//  Number gcd(const Polynom&, const Number&);
//  Number gcd(const Number&, const Polynom&);
//  Polynom lcm(const Polynom&, const Polynom&);
//  Polynom abs(const Polynom&);
// 
// #include <Polynom.inline.h>
// #include <Polynom_comparison.inline.h>
// #include <Polynom_addition.inline.h>
// #include <Polynom_multiplication.inline.h>

#endif /* __POLYNOM_H__ */
