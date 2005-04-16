#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

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
		void erase() { myLength = 0; }
		const string &operator += (const string &s);
		const string &operator += (char c);

		size_t length() const { return myLength; }
		const char *data() const { return myData; }

		int compare(const string &s, int fromPos, int len) const;
		bool operator == (const string &s) const;
		
		size_t find(const string &pattern, size_t fromPos) const;

	private:
		size_t myLength;
		size_t myDataSize;
		char *myData;
	};
};

#endif /* __STRING_H__ */
