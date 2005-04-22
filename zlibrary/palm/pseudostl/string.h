#ifndef __STRING_H__
#define __STRING_H__

#include <sys_types.h>
#include <unix_string.h>
#include <Core/System/StringMgr.h>
#include <Core/System/MemoryMgr.h>

namespace std {
	class string {
	
	public:

		string();
		~string();
		string(const string &s);
		string(const char *s);
		const string &operator = (const string &s);
		const string &operator = (const char *s);
		
		void reserve(size_t minSize);
		void erase(size_t start = 0, size_t len = (size_t)-1);
		const string &operator += (const string &s);
		const string &operator += (char c);
		const string operator + (const string &s) const;

		size_t length() const { return myLength; }
		const char *data() const { return myData; }
		const char *c_str();

		string substr(size_t start, size_t len = (size_t)-1) const;
		void append(const char *s, size_t len);

		int compare(const string &s, int fromPos, int len) const;
		bool operator == (const string &s) const;
		
		size_t find(const string &pattern, size_t fromPos) const;
		size_t rfind(const string &pattern, size_t fromPos) const;
		size_t find(char c) const;
		size_t rfind(char c) const;

	private:
		size_t myLength;
		size_t myDataSize;
		char *myData;
	};

	const string operator + (const char *s0, const string &s1);
};

#endif /* __STRING_H__ */
