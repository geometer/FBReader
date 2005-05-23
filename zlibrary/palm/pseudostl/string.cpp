#include <algorithm>

#include "string1.h"

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
		__myLength = 0;
		__myDataSize = STRING_TINY_DATA_SIZE;
		__myData = __myTinyData;
	}

	string::~string() {
		if (__myData != __myTinyData) {
			MemPtrFree(__myData);
		}
	}

	string::string(const string &s) {
		__myDataSize = STRING_TINY_DATA_SIZE;
		__myData = __myTinyData;
		operator = (s);
	}

	string::string(const char *s) {
		__myLength = 0;
		__myDataSize = STRING_TINY_DATA_SIZE;
		__myData = __myTinyData;

		size_t sLength = StrLen(s);
		if (sLength > 0) {
			reserve(sLength);
			MemMove(__myData, s, sLength);
			__myLength = sLength;
		}
	}

	const char *string::c_str() const {
		reserve(__myLength + 1);
		__myData[__myLength] = '\0';
		return __myData;
	}

	const string &string::operator = (const string &s) {
		__myLength = 0;
		if (s.__myLength > 0) {
			reserve(s.__myLength);
			__myLength = s.__myLength;
			MemMove(__myData, s.__myData, __myLength);
		}
		return *this;
	}

	const string &string::operator = (const char *s) {
		__myLength = 0;
		size_t len = StrLen(s);
		if (len > 0) {
			reserve(len);
			__myLength = len;
			MemMove(__myData, s, __myLength);
		}
		return *this;
	}

	bool string::operator == (const string &s) const {
		return (__myLength == s.__myLength) && (strncmp(__myData, s.__myData, __myLength) == 0);
	}

	bool string::operator == (const char *s) const {
		return (__myLength == StrLen(s)) && (strncmp(__myData, s, __myLength - 1) == 0);
	}

	bool string::operator < (const string &s) const {
		size_t len = std::min(__myLength, s.__myLength);
		int result = strncmp(__myData, s.__myData, len);
		if (result < 0) {
			return true;
		} else if (result == 0) {
			return __myLength < s.__myLength;
		}
		return false;
	}

	void string::swap(string &s) {
		if ((__myData != __myTinyData) && (s.__myData != s.__myTinyData)) {
			char *p = __myData;
			__myData = s.__myData;
			s.__myData = p;
			int l = __myDataSize;
			__myDataSize = s.__myDataSize;
			s.__myDataSize = l;
			l = __myLength;
			__myLength = s.__myLength;
			s.__myLength = l;
		} else {
			// TODO: implement
		}
	}

	const string &string::operator += (const string &s) {
		reserve(__myLength + s.__myLength);
		MemMove(__myData + __myLength, s.__myData, s.__myLength);
		__myLength += s.__myLength;
		return *this;
	}

	const string &string::operator += (char c) {
		reserve(__myLength + 1);
		__myData[__myLength] = c;
		__myLength++;
		return *this;
	}

	int string::compare(const string &s, int fromPos, int len) const {
		// TODO: check for short string length < len (?)
		const char *ptr0 = __myData + fromPos;
		const char *ptr1 = s.__myData;
		const char *end = ptr0 + len;
		for (; ptr0 != end; ptr0++, ptr1++) {
			if (*ptr0 < *ptr1) {
				return -1;
			}
			if (*ptr0 > *ptr1) {
				return 1;
			}
		}
		
		return 0;
	}
		
	size_t string::find(const string &pattern, size_t fromPos) const {
		// TODO: implement
		return 0;
	}

	size_t string::find(char c, size_t fromPos) const {
		if (fromPos > __myLength) {
			return (size_t)-1;
		}

		char *end = __myData + __myLength;
		for (char *ptr = __myData + fromPos; ptr != end; ++ptr) {
			if (*ptr == c) {
				return ptr - __myData;
			}
		}
		return (size_t)-1;
	}

	size_t string::rfind(char c, size_t fromPos) const {
		if (fromPos > __myLength) {
			fromPos = __myLength;
		}
		char *end = __myData - 1;
		for (char *ptr = __myData + fromPos - 1; ptr != end; --ptr) {
			if (*ptr == c) {
				return ptr - __myData;
			}
		}
		return (size_t)-1;
	}

	string string::substr(size_t start, size_t len = (size_t)-1) const {
		if (start >= __myLength) {
			len = 0;
		} else if (len > __myLength - start) {
			len = __myLength - start;
		}
		string result;
		result.reserve(len);
		MemMove(result.__myData, __myData + start, len);
		result.__myLength = len;
		return result;
	}

	void string::erase(size_t start, size_t len) {
		if ((len == (size_t)-1) || (start + len >= __myLength)) {
			if (start < __myLength) {
				__myLength = start;
			}
		} else {
			__myLength -= len;
			MemMove(__myData + start, __myData + start + len, __myLength - start);
		}
	}

	const string string::operator + (const string &s) const {
		string sum;
		sum.reserve(__myLength + s.__myLength);
		MemMove(sum.__myData, __myData, __myLength);
		MemMove(sum.__myData + __myLength, s.__myData, s.__myLength);
		sum.__myLength = __myLength + s.__myLength;
		return sum;
	}

	const string string::operator + (char c) const {
		string sum;
		sum.reserve(__myLength + 1);
		MemMove(sum.__myData, __myData, __myLength);
		sum.__myData[__myLength] = c;
		sum.__myLength = __myLength + 1;
		return sum;
	}

	const string operator + (const char *s0, const string &s1) {
		string sum;
		int len0 = StrLen(s0);
		sum.reserve(len0 + s1.__myLength);
		MemMove(sum.__myData, s0, len0);
		MemMove(sum.__myData + len0, s1.__myData, s1.__myLength);
		sum.__myLength = len0 + s1.__myLength;
		return sum;
	}

	void string::reserve(size_t minSize) const {
		if (minSize > __myDataSize) {
			while (minSize > __myDataSize) {
				__myDataSize <<= 1;
			}
			if (__myData == __myTinyData) {
				__myData = (char*)MemPtrNew(__myDataSize);
				if (__myLength > 0) {
					MemMove(__myData, __myTinyData, __myLength);
				}
			} else if (MemPtrResize(__myData, __myDataSize) != errNone) {
				char *d = (char*)MemPtrNew(__myDataSize);
				if (__myLength > 0) {
					MemMove(d, __myData, __myLength);
				}
				MemPtrFree(__myData);
				__myData = d;
			}
		}
	}

	void string::append(const char *s, size_t len) {
		reserve(__myLength + len);
		MemMove(__myData + __myLength, s, len);
		__myLength += len;
	}
};
