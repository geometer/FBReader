#ifndef __STRING_H__
#define __STRING_H__

#include <sys_types.h>
#include <Core/System/StringMgr.h>
#include <Core/System/MemoryMgr.h>

#define		memset(x,y,z)			MemSet(x,z,y)
#define		strcasecmp(x,y)		StrCaselessCompare(x,y)
#define		strncpy(x,y,z)		MemMove(x,y,z)

//int strncpy(char *dst, const char *src, size_t len) STL1_SECTION;

#define TINY_DATA_SIZE 16

namespace std {
	class string {
	
	public:
		string() STL1_SECTION;
		~string() STL1_SECTION;
		string(const string &s) STL1_SECTION;
		string(const char *s) STL1_SECTION;
		const string &operator = (const string &s) STL1_SECTION;
		const string &operator = (const char *s) STL1_SECTION;
		
		void reserve(size_t minSize) const STL1_SECTION;
		void erase(size_t start = 0, size_t len = (size_t)-1) STL1_SECTION;
		const string &operator += (const string &s) STL1_SECTION;
		const string &operator += (char c) STL1_SECTION;
		const string operator + (const string &s) const STL1_SECTION;
		const string operator + (char c) const STL1_SECTION;

		size_t length() const STL1_SECTION;
		bool empty() const STL1_SECTION;
		const char *data() const STL1_SECTION;
		const char *c_str() const STL1_SECTION;
		char &operator [] (size_t index) const STL1_SECTION;

		string substr(size_t start, size_t len = (size_t)-1) const STL1_SECTION;
		void append(const char *s, size_t len) STL1_SECTION;
		void swap(string &s) STL1_SECTION;

		int compare(const string &s, int fromPos, int len) const STL1_SECTION;
		bool operator == (const string &s) const STL1_SECTION;
		bool operator != (const string &s) const STL1_SECTION;
		bool operator == (const char *s) const STL1_SECTION;
		bool operator != (const char *s) const STL1_SECTION;
		bool operator < (const string &s) const STL1_SECTION;
		
		size_t find(const string &pattern, size_t fromPos = 0) const STL1_SECTION;
		size_t rfind(const string &pattern, size_t fromPos = (size_t)-1) const STL1_SECTION;
		size_t find(char c, size_t fromPos = 0) const STL1_SECTION;
		size_t rfind(char c, size_t fromPos = (size_t)-1) const STL1_SECTION;

	private:
		size_t __myLength;
		mutable size_t __myDataSize;
		mutable char *__myData;
		mutable char __myTinyData[TINY_DATA_SIZE];

	friend const string operator + (const char *s0, const string &s1) STL1_SECTION;
	};

	const string operator + (const char *s0, const string &s1) STL1_SECTION;
	bool operator == (const char *s0, const string &s1) STL1_SECTION;
	bool operator != (const char *s0, const string &s1) STL1_SECTION;

	inline size_t string::length() const { return __myLength; }
	inline bool string::empty() const { return __myLength == 0; }
	inline const char *string::data() const { return __myData; }
	inline char &string::operator [] (size_t index) const { return __myData[index]; }
	inline bool string::operator != (const string &s) const { return !operator == (s); }
	inline bool string::operator != (const char *s) const { return !operator == (s); }
	inline bool operator == (const char *s0, const string &s1) { return s1 == s0; }
	inline bool operator != (const char *s0, const string &s1) { return s1 != s0; }
};

#endif /* __STRING_H__ */
