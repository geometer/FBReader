/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLCHARSEQUENCE_H__
#define __ZLCHARSEQUENCE_H__

#include <string>

class ZLCharSequence {

public:
	ZLCharSequence(); 
	ZLCharSequence(const char *ptr, size_t size); 
	ZLCharSequence(const std::string &hexSequence);
	ZLCharSequence(const ZLCharSequence &other);
	~ZLCharSequence();		
 
	size_t getSize() const;
	const char &operator [] (size_t index) const;
	ZLCharSequence &operator = (const ZLCharSequence& other);
		
	std::string toString() const; 
	std::string toHexSequence() const; 

	// returns
	//   an integer < 0 if the sequence is less than other
	//   an integer > 0 if the sequence is greater than other
	//   0 if the sequence is equal to other
	int compareTo(const ZLCharSequence &other) const;

private: 
	size_t mySize;
	char *myHead;
};

inline ZLCharSequence::ZLCharSequence() : mySize(0), myHead(0) {}

inline const char& ZLCharSequence::operator [] (size_t index) const {
	return myHead[index];
}

inline bool operator < (const ZLCharSequence &a, const ZLCharSequence &b) {
	return a.compareTo(b) < 0;
}

inline int ZLCharSequence::compareTo(const ZLCharSequence &other) const {
	int difference = mySize - other.mySize;
	if (difference != 0) {
		return difference;
	}
	for (size_t i = 0; i < mySize; ++i) {
		difference = myHead[i] - other.myHead[i];
		if (difference != 0) {
			return difference;
		}
	}
	return 0;
}

inline ZLCharSequence::~ZLCharSequence() {
	delete[] myHead;
}

inline size_t ZLCharSequence::getSize() const {
	return mySize;
}

inline std::string ZLCharSequence::toString() const {
	return std::string(myHead, 0, mySize);
}

#endif /*__ZLCHARSEQUENCE_H__*/
