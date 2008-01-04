/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include <string>

class BitStream {

public:
	static const int BufferSize;

public:
	BitStream();
	void setData(const std::string &data);
	void reset();
	unsigned int peek(unsigned char length);
	void remove(unsigned char length);
	unsigned int get(unsigned char length);
	unsigned int bytesLeft() const;

	unsigned int get4BytesDirect();
	bool getBytesDirect(unsigned char *buffer, unsigned int length);

private:
	bool ensure(unsigned char length);

private:
	unsigned int myBuffer;
	unsigned char myBitCounter;
	const unsigned char *myByteStream;
	const unsigned char *myByteStreamEnd;

private:
	BitStream(const BitStream&);
	const BitStream &operator = (const BitStream&);
};

inline BitStream::BitStream() : myBuffer(0), myBitCounter(0) {
}

inline void BitStream::setData(const std::string &data) {
	myByteStream = (const unsigned char*)data.data();
	myByteStreamEnd = myByteStream + data.length();
	myBuffer = 0;
	myBitCounter = 0;
}

inline void BitStream::reset() {
	myByteStream -= myBitCounter / 8;
	myBuffer = 0;
	myBitCounter = 0;
}

inline bool BitStream::ensure(unsigned char length) {
	while ((myBitCounter < length) && (bytesLeft() >= 2)) {
		myBuffer |= ((myByteStream[1] << 8) | myByteStream[0]) << (BitStream::BufferSize - 16 - myBitCounter);
		myBitCounter += 16;
		myByteStream += 2;
	}
	return myBitCounter >= length;
}

inline unsigned int BitStream::peek(unsigned char length) {
	ensure(length);
	return (length > 0) ? (myBuffer >> (BufferSize - length)) : 0;
}

inline void BitStream::remove(unsigned char length) {
	if (ensure(length)) {
		myBuffer <<= length;
		myBitCounter -= length;
	}
}

inline unsigned int BitStream::get(unsigned char length) {
	unsigned int bits;
	if (length > 16) {
		bits = peek(length - 16) << 16;
		remove(length - 16);
		bits += peek(16);
		remove(16);
	} else {
		bits = peek(length);
		remove(length);
	}
	return bits;
}

inline unsigned int BitStream::bytesLeft() const {
	return myByteStreamEnd - myByteStream;
}

#endif /* __BITSTREAM_H__ */
