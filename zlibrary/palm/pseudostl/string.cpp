#include "string.h"

// TODO: optimize (use more palm-specific functions, e.g., instead of new and delete[])
namespace std {

	string::string() {
		myLength = 0;
		myDataSize = 0;
		myData = 0;
	}

	string::~string() {
		if (myData != 0) {
			delete[] myData;
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
			StrNCopy(myData, s, sLength);
			myLength = sLength;
		}
	}

	const char *string::c_str() {
		reserve(myLength + 1);
		myData[myLength] = '\0';
		return myData;
	}

	const string &string::operator = (const string &s) {
		myLength = 0;
		if (s.myLength > 0) {
			reserve(s.myLength);
			myLength = s.myLength;
			StrNCopy(myData, s.myData, myLength);
		}
		return *this;
	}

	void string::reserve(size_t minSize) {
		if (minSize > myDataSize) {
			if (minSize | 0xf) {
				minSize = (minSize & (size_t)-0x10) + 0x10;
			}
			myDataSize = minSize;
			char *d = new char[myDataSize];
			if (myLength > 0) {
				StrNCopy(d, myData, myLength);
			}
			if (myData != 0) {
				delete[] myData;
			}
			myData = d;
		}
	}

	bool string::operator == (const string &s) const {
		if (myLength != s.myLength) {
			return false;
		}
		return StrNCompare(myData, s.myData, myLength) == 0;
	}

	const string &string::operator += (const string &s) {
		// TODO: implement
		return *this;
	}

	const string &string::operator += (char c) {
		// TODO: implement
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

	size_t string::find(char c) const {
		// TODO: implement
		return 0;
	}

	size_t string::rfind(char c) const {
		// TODO: implement
		return 0;
	}

	string string::substr(size_t start, size_t len = (size_t)-1) const {
		// TODO: implement
		return string();
	}

	void string::append(const char *s, size_t len) {
		// TODO: implement
	}

	void string::erase(size_t start, size_t len) {
		if ((len == (size_t)-1) || (start + len >= myLength)) {
			if (start < myLength) {
				myLength = start;
			}
		} else {
			myLength -= len;
			StrNCopy(myData + start, myData + start + len, myLength - start);
		}
	}

	const string string::operator + (const string &s) const {
		// TODO: implement
		return *this;
	}

	const string operator + (const char *s0, const string &s1) {
		// TODO: implement
		return s1;
	}
};
