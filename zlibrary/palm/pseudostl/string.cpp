#include <algorithm>

#include "string.h"

static int strncmp(const char *s1, const char *s2, size_t len) STL_SECTION;
static int strncmp(const char *s1, const char *s2, size_t len) {
	const char *e1 = s1 + len;
	for (; s1 != e1; s1++, s2++) {
		if (*s1 < *s2) {
			return -1;
		} else if (*s1 > *s2) {
			return 1;
		}
	}
	return 0;
}

namespace std {

	string::string() {
		myLength = 0;
		myDataSize = 0;
		myData = 0;
	}

	string::~string() {
		if (myData != 0) {
			MemPtrFree(myData);
		}
	}

	string::string(const string &s) {
		myDataSize = 0;
		myData = 0;
		operator = (s);
	}

	string::string(const char *s) {
		myLength = 0;
		myDataSize = 0;
		myData = 0;

		size_t sLength = StrLen(s);
		if (sLength > 0) {
			reserve(sLength);
			MemMove(myData, s, sLength);
			myLength = sLength;
		}
	}

	const char *string::c_str() const {
		const_reserve(myLength + 1);
		myData[myLength] = '\0';
		return myData;
	}

	const string &string::operator = (const string &s) {
		myLength = 0;
		if (s.myLength > 0) {
			reserve(s.myLength);
			myLength = s.myLength;
			MemMove(myData, s.myData, myLength);
		}
		return *this;
	}

	const string &string::operator = (const char *s) {
		myLength = 0;
		size_t len = StrLen(s);
		if (len > 0) {
			reserve(len);
			myLength = len;
			MemMove(myData, s, myLength);
		}
		return *this;
	}

	void string::const_reserve(size_t minSize) const {
		if (minSize > myDataSize) {
			if (minSize | 0xf) {
				minSize = (minSize & (size_t)-0x10) + 0x10;
			}
			myDataSize = minSize;
			// TODO: use MemPtrResize
			char *d = (char*)MemPtrNew(myDataSize);
			if (myLength > 0) {
				MemMove(d, myData, myLength);
			}
			if (myData != 0) {
				MemPtrFree(myData);
			}
			myData = d;
		}
	}

	bool string::operator == (const string &s) const {
		return (myLength == s.myLength) && (strncmp(myData, s.myData, myLength) == 0);
	}

	bool string::operator == (const char *s) const {
		return (myLength == StrLen(s)) && (strncmp(myData, s, myLength - 1) == 0);
	}

	bool string::operator < (const string &s) const {
		size_t len = std::min(myLength, s.myLength);
		int result = strncmp(myData, s.myData, len);
		if (result < 0) {
			return true;
		} else if (result == 0) {
			return myLength < s.myLength;
		}
		return false;
	}

	void string::append(const char *s, size_t len) {
		reserve(myLength + len);
		MemMove(myData + myLength, s, len);
		myLength += len;
	}

	void string::swap(string &s) {
		char *p = myData;
		myData = s.myData;
		s.myData = p;
		int l = myDataSize;
		myDataSize = s.myDataSize;
		s.myDataSize = l;
		l = myLength;
		myLength = s.myLength;
		s.myLength = l;
	}

	const string &string::operator += (const string &s) {
		reserve(myLength + s.myLength);
		MemMove(myData + myLength, s.myData, s.myLength);
		myLength += s.myLength;
		return *this;
	}

	const string &string::operator += (char c) {
		reserve(myLength + 1);
		myData[myLength] = c;
		myLength++;
		return *this;
	}

	int string::compare(const string &s, int fromPos, int len) const {
		// TODO: implement
		return 0;
	}
		
	size_t string::find(const string &pattern, size_t fromPos) const {
		// TODO: implement
		return 0;
	}

	size_t string::find(char c, size_t fromPos) const {
		if (fromPos > myLength) {
			return (size_t)-1;
		}

		char *end = myData + myLength;
		for (char *ptr = myData + fromPos; ptr != end; ++ptr) {
			if (*ptr == c) {
				return ptr - myData;
			}
		}
		return (size_t)-1;
	}

	size_t string::rfind(char c, size_t fromPos) const {
		if (fromPos > myLength) {
			fromPos = myLength;
		}
		char *end = myData - 1;
		for (char *ptr = myData + fromPos - 1; ptr != end; --ptr) {
			if (*ptr == c) {
				return ptr - myData;
			}
		}
		return (size_t)-1;
	}

	string string::substr(size_t start, size_t len = (size_t)-1) const {
		if (start >= myLength) {
			len = 0;
		} else if (len > myLength - start) {
			len = myLength - start;
		}
		string result;
		result.reserve(len);
		MemMove(result.myData, myData + start, len);
		result.myLength = len;
		return result;
	}

	void string::erase(size_t start, size_t len) {
		if ((len == (size_t)-1) || (start + len >= myLength)) {
			if (start < myLength) {
				myLength = start;
			}
		} else {
			myLength -= len;
			MemMove(myData + start, myData + start + len, myLength - start);
		}
	}

	const string string::operator + (const string &s) const {
		string sum;
		sum.reserve(myLength + s.myLength);
		MemMove(sum.myData, myData, myLength);
		MemMove(sum.myData + myLength, s.myData, s.myLength);
		sum.myLength = myLength + s.myLength;
		return sum;
	}

	const string string::operator + (char c) const {
		string sum;
		sum.reserve(myLength + 1);
		MemMove(sum.myData, myData, myLength);
		sum.myData[myLength] = c;
		sum.myLength = myLength + 1;
		return sum;
	}

	const string operator + (const char *s0, const string &s1) {
		string sum;
		int len0 = StrLen(s0);
		sum.reserve(len0 + s1.myLength);
		MemMove(sum.myData, s0, len0);
		MemMove(sum.myData + len0, s1.myData, s1.myLength);
		sum.myLength = len0 + s1.myLength;
		return sum;
	}
};
