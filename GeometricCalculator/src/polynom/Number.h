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

#ifndef __NUMBER_H__
#define __NUMBER_H__

class Number {
	private:
		int myValue;

	public:
		Number(int value = 0) { myValue = value; }
		Number(const Number &number) { myValue = number.myValue; }

		const Number& operator = (int value) { myValue = value; return *this; }
		const Number& operator = (const Number &number) { myValue = number.myValue; return *this; }

		Number& operator += (int value) { myValue += value; return *this; }
		Number& operator += (const Number &number) { myValue += number.myValue; return *this; }
		Number& operator -= (int value) { myValue -= value; return *this; }
		Number& operator -= (const Number &number) { myValue -= number.myValue; return *this; }
		Number& operator *= (int value) { myValue *= value; return *this; }
		Number& operator *= (const Number &number) { myValue *= number.myValue; return *this; }
		Number& operator /= (int value) { myValue /= value; return *this; }
		Number& operator /= (const Number &number) { myValue /= number.myValue; return *this; }
		Number& operator %= (int value) { myValue %= value; return *this; }
		Number& operator %= (const Number &number) { myValue %= number.myValue; return *this; }

		bool operator == (int value) const { return myValue == value; }
		bool operator == (const Number &number) const { return myValue == number.myValue; }
		bool operator < (const Number &number) const { return myValue < number.myValue; }
		bool operator < (int value) const { return myValue < value; }
		bool operator > (int value) const { return myValue > value; }

		unsigned int strlen() const;
		const char* sprintf(char *buffer) const;
};

/* Unary '-' and '+' */
// TODO: optimize
inline Number operator - (Number number) { return number *= -1; }
inline const Number& operator + (const Number &number) { return number; }

/* Binary arithmetic operators */
inline Number operator + (Number number1, const Number &number2) { return number1 += number2; }
inline Number operator + (Number number, int value) { return number += value; }
inline Number operator + (int value, Number number) { return number += value; }
inline Number operator - (Number number1, const Number &number2) { return number1 -= number2; }
inline Number operator - (Number number, int value) { return number -= value; }
inline Number operator - (int value, Number number) { return -number += value; }
inline Number operator * (Number number1, const Number &number2) { return number1 *= number2; }
inline Number operator * (Number number, int value) { return number *= value; }
inline Number operator * (int value, Number number) { return number *= value; }
inline Number operator / (Number number1, const Number &number2) { return number1 /= number2; }
inline Number operator / (Number number, int value) { return number /= value; }
//Number operator / (int value, Number number) { return number /= value; }
inline Number operator % (Number number1, const Number &number2) { return number1 %= number2; }
inline Number operator % (Number number, int value) { return number %= value; }
//Number operator % (int value, Number number) { return number %= value; }

/* Equality operators */
inline bool operator == (int value, const Number &number) { return number == value; }
inline bool operator != (const Number &number1, const Number &number2) { return !(number1 == number2); }
inline bool operator != (int value, const Number &number) { return !(number == value); }
inline bool operator != (const Number &number, int value) { return !(number == value); }

/* Comparison operators */
inline bool operator < (int value, const Number number) { return number > value; }
inline bool operator > (const Number &number1, const Number &number2) { return number2 < number1; }
inline bool operator > (int value, const Number number) { return number < value; }
inline bool operator <= (const Number &number1, const Number &number2) { return !(number2 < number1); }
inline bool operator <= (int value, const Number number) { return !(number < value); }
inline bool operator <= (const Number number, int value) { return !(number > value); }
inline bool operator >= (const Number &number1, const Number &number2) { return !(number1 < number2); }
inline bool operator >= (int value, const Number number) { return !(number > value); }
inline bool operator >= (const Number number, int value) { return !(number < value); }

inline Number abs(const Number &number) { return (number >= 0) ? number : -number; }

#endif /* __NUMBER_H__ */
