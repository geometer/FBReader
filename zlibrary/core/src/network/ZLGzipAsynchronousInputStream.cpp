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


#include <string.h>

#include "ZLGzipAsynchronousInputStream.h"


static const size_t OUT_BUFFER_SIZE = 32768;


ZLGzipAsynchronousInputStream::ZLGzipAsynchronousInputStream(const char *encoding) : ZLAsynchronousInputStream(encoding), 
		myOutBufferSize(OUT_BUFFER_SIZE), myEndOfStream(false), myStreamState(1), myOffset(0) {

	myZStream = new z_stream;
	memset(myZStream, 0, sizeof(z_stream));
	inflateInit2(myZStream, -MAX_WBITS);

	myOutBuffer = new char[OUT_BUFFER_SIZE];
}

ZLGzipAsynchronousInputStream::~ZLGzipAsynchronousInputStream() {
	delete[] myOutBuffer;

	inflateEnd(myZStream);
	delete myZStream;
}


bool ZLGzipAsynchronousInputStream::processInputInternal(Handler &handler) {
	if (myEndOfStream) {
		return false;
	}
	if (eof()) {
		handler.shutdown();
		myEndOfStream = true;
		return true;
	}
	while (myOffset < myDataLen && myStreamState) {
		if (!skipHeader(myData, myDataLen)) {
			return false;
		}
	}
	if (myOffset >= myDataLen) {
		myOffset -= myDataLen;
		return true;
	}
	myZStream->next_in = (Bytef*) myData + myOffset;
	myZStream->avail_in = myDataLen - myOffset;
	myOffset = 0;
	bool forcedCall = false;
	while (!myEndOfStream && (myZStream->avail_in > 0 || forcedCall)) {
		forcedCall = false;
		myZStream->avail_out = myOutBufferSize;
		myZStream->next_out = (Bytef*) myOutBuffer;
		int code = ::inflate(myZStream, Z_SYNC_FLUSH);
		if ((code != Z_OK) && (code != Z_STREAM_END)) {
			return false;
		}
		if (myOutBufferSize != myZStream->avail_out) {
			if (myZStream->avail_out == 0) {
				forcedCall = true;
			}
			if (!handler.handleBuffer(myOutBuffer, myOutBufferSize - myZStream->avail_out)) {
				return false;
			}
			if (code == Z_STREAM_END) {
				myEndOfStream = true;
				setEof();
				handler.shutdown();
			}
		}
	}
	return true;
}


bool ZLGzipAsynchronousInputStream::skipHeader(const char *data, size_t /*len*/) {
	static const unsigned char FHCRC = 1 << 1;
	static const unsigned char FEXTRA = 1 << 2;
	static const unsigned char FNAME = 1 << 3;
	static const unsigned char FCOMMENT = 1 << 4;
	unsigned char b;
	switch (myStreamState) {
	case 1:
		b = data[myOffset++];
		if (b != 31) return false;
		break;
	case 2:
		b = data[myOffset++];
		if (b != 139) return false;
		break;
	case 3:
		b = data[myOffset++];
		if (b != 8) return false;
		break;
	case 4:
		myFlag = data[myOffset++];
		myOffset += 6;
		break;
	case 5:
		if (myFlag & FEXTRA) {
			myByte = data[myOffset++];
		}
		break;
	case 6:
		if (myFlag & FEXTRA) {
			b = data[myOffset++];
			unsigned short xlen = (((unsigned short) b) << 8) + myByte;
			myOffset += xlen;
		}
		break;
	case 7:
		if (myFlag & FNAME) {
			b = data[myOffset++];
			if (b != 0) return true;
		}
		break;
	case 8:
		if (myFlag & FCOMMENT) {
			b = data[myOffset++];
			if (b != 0) return true;
		}
		break;
	case 9:
		if (myFlag & FHCRC) {
			myOffset += 2;
		}
		break;
	case 10: // must be line with maximum case value
		myStreamState = 0;
		return true;
	default:
		return false;
	}
	//std::cout << "SIZE == " << inSize << std::endl;
	//std::cout << "OFFSET == " << inOffset << std::endl;
	++myStreamState;
	return true;
}

