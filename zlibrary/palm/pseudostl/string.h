#ifndef __STRING_H__
#define __STRING_H__

#include <sys_types.h>
#include <Core/System/StringMgr.h>
#include <Core/System/MemoryMgr.h>

#define		memset(x,y,z)			MemSet(x,z,y)
#define		strcasecmp(x,y)		StrCaselessCompare(x,y)

int strncpy(char *dst, const char *src, size_t len) STL_SECTION;

namespace std {
	class string {
	
	public:
		string() STL_SECTION;
		~string() STL_SECTION;
		string(const string &s) STL_SECTION;
		string(const char *s) STL_SECTION;
		const string &operator = (const string &s) STL_SECTION;
		const string &operator = (const char *s) STL_SECTION;
		
		void reserve(size_t minSize) STL_SECTION;
		void erase(size_t start = 0, size_t len = (size_t)-1) STL_SECTION;
		const string &operator += (const string &s) STL_SECTION;
		const string &operator += (char c) STL_SECTION;
		const string operator + (const string &s) const STL_SECTION;
		const string operator + (char c) const STL_SECTION;

		size_t length() const STL_SECTION;
		bool empty() const STL_SECTION;
		const char *data() const STL_SECTION;
		const char *c_str() const STL_SECTION;
		char &operator [] (size_t index) const STL_SECTION;

		string substr(size_t start, size_t len = (size_t)-1) const STL_SECTION;
		void append(const char *s, size_t len) STL_SECTION;
		void swap(string &s) STL_SECTION;

		int compare(const string &s, int fromPos, int len) const STL_SECTION;
		bool operator == (const string &s) const STL_SECTION;
		bool operator != (const string &s) const STL_SECTION;
		bool operator == (const char *s) const STL_SECTION;
		bool operator != (const char *s) const STL_SECTION;
		bool operator < (const string &s) const STL_SECTION;
		
		size_t find(const string &pattern, size_t fromPos = 0) const STL_SECTION;
		size_t rfind(const string &pattern, size_t fromPos = (size_t)-1) const STL_SECTION;
		size_t find(char c, size_t fromPos = 0) const STL_SECTION;
		size_t rfind(char c, size_t fromPos = (size_t)-1) const STL_SECTION;

	private:
		void const_reserve(size_t minSize) const STL_SECTION;

	private:
		size_t myLength;
		mutable size_t myDataSize;
		mutable char *myData;

	friend const string operator + (const char *s0, const string &s1) STL_SECTION;
	};

	const string operator + (const char *s0, const string &s1) STL_SECTION;
	bool operator == (const char *s0, const string &s1) STL_SECTION;
	bool operator != (const char *s0, const string &s1) STL_SECTION;

	inline size_t string::length() const { return myLength; }
	inline bool string::empty() const { return myLength == 0; }
	inline const char *string::data() const { return myData; }
	inline char &string::operator [] (size_t index) const { return myData[index]; }
	inline bool string::operator != (const string &s) const { return !operator == (s); }
	inline bool string::operator != (const char *s) const { return !operator == (s); }
	inline bool operator == (const char *s0, const string &s1) { return s1 == s0; }
	inline bool operator != (const char *s0, const string &s1) { return s1 != s0; }
	inline void string::reserve(size_t minSize) { const_reserve(minSize); }
};

#endif /* __STRING_H__ */
