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
