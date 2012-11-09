/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <cstring>

#include <ZLInputStream.h>

#include "PdbReader.h"
#include "BitReader.h"
#include "HuffDecompressor.h"

HuffDecompressor::HuffDecompressor(ZLInputStream& stream, 
                        const std::vector<unsigned long>::const_iterator beginIt, 
                        const std::vector<unsigned long>::const_iterator endIt,
						const unsigned long endHuffDataOffset, const unsigned long extraFlags) : myExtraFlags(extraFlags), myErrorCode(ERROR_NONE) {
	

	const unsigned long huffHeaderOffset = *beginIt;
	const unsigned long huffRecordsNumber = endIt - beginIt;
	const unsigned long huffDataOffset = *(beginIt + 1);
	
	stream.seek(huffHeaderOffset, true);
	stream.seek(16, false);
	unsigned long cacheTableOffset, baseTableOffset;
	PdbUtil::readUnsignedLongBE(stream, cacheTableOffset);
	PdbUtil::readUnsignedLongBE(stream, baseTableOffset); 
	

	myCacheTable = new unsigned long[256];
	stream.seek(huffHeaderOffset + cacheTableOffset, true);
	for (std::size_t i = 0; i < 256; ++i) {
		PdbUtil::readUnsignedLongLE(stream, myCacheTable[i]); //LE
	}
	
	myBaseTable = new unsigned long[64]; 
	stream.seek(huffHeaderOffset + baseTableOffset, true);
	for (std::size_t i = 0; i < 64; ++i) {
		PdbUtil::readUnsignedLongLE(stream, myBaseTable[i]); //LE
	}
	
	stream.seek(huffDataOffset + 12, true);
	PdbUtil::readUnsignedLongBE(stream, myEntryBits);	
	
	std::size_t huffDataSize = endHuffDataOffset - huffDataOffset;
	myData = new unsigned char[huffDataSize];
	stream.seek(huffDataOffset, true);
	if (huffDataSize == stream.read((char*)myData, huffDataSize)) {	
		myDicts = new unsigned char* [huffRecordsNumber - 1];
		for(std::size_t i = 0; i < huffRecordsNumber - 1;  ++i) {	
			std::size_t shift = *(beginIt + i + 1) - huffDataOffset;
			myDicts[i] = myData + shift;
		}
	} else {
		myErrorCode = ERROR_CORRUPTED_FILE;
	}
	
	myTargetBuffer = 0;
	myTargetBufferEnd = 0;
	myTargetBufferPtr = 0;
}

HuffDecompressor::~HuffDecompressor() {
	delete[] myCacheTable;
	delete[] myBaseTable;
	delete[] myData;
	delete[] myDicts;
}

bool HuffDecompressor::error() const {
	return myErrorCode == ERROR_CORRUPTED_FILE;
}

std::size_t HuffDecompressor::decompress(ZLInputStream &stream, char *targetBuffer, std::size_t compressedSize, std::size_t maxUncompressedSize) {
	if ((compressedSize == 0) || (myErrorCode == ERROR_CORRUPTED_FILE)) {
		return 0;
	}
	if (targetBuffer != 0) { 
		unsigned char *sourceBuffer = new unsigned char[compressedSize];
    	myTargetBuffer = targetBuffer;
		myTargetBufferEnd = targetBuffer + maxUncompressedSize;
    	myTargetBufferPtr = targetBuffer;
		if (stream.read((char*)sourceBuffer, compressedSize) == compressedSize) {
			std::size_t trailSize = sizeOfTrailingEntries(sourceBuffer, compressedSize); 
        	if (trailSize < compressedSize) { 
				bitsDecompress(BitReader(sourceBuffer, compressedSize - trailSize));
			} else {
				myErrorCode = ERROR_CORRUPTED_FILE;
			}
		}
		delete[] sourceBuffer;
	} else {
		myTargetBuffer = 0;
		myTargetBufferEnd = 0;
		myTargetBufferPtr = 0;
	}

	return myTargetBufferPtr - myTargetBuffer;
}

void HuffDecompressor::bitsDecompress(BitReader bits, std::size_t depth) {
	if (depth > 32) {
		myErrorCode = ERROR_CORRUPTED_FILE;
		return;
	}	

	while (bits.left()) {
    	const unsigned long dw = (unsigned long)bits.peek(32);
	    const unsigned long v = myCacheTable[dw >> 24];
  	 	unsigned long codelen = v & 0x1F;
		//if ((codelen == 0) || (codelen > 32)) {
		//	return false;
		//} 
		unsigned long code = dw >> (32 - codelen);
    	unsigned long r = (v >> 8);
    	if (!(v & 0x80)) {
        	while (code < myBaseTable[(codelen - 1) * 2]) {
            	codelen += 1;
            	code = dw >> (32 - codelen);
			}
        	r = myBaseTable[(codelen - 1) * 2 + 1];
		}
    	r -= code;
		//if (codelen == 0) {
		//	return false;
		//} 
		if (!bits.eat(codelen)) {
			return;
		}
    	const unsigned long dicno = r >> myEntryBits;
    	const unsigned long off1 = 16 + (r - (dicno << myEntryBits)) * 2;
    	const unsigned char* dict = myDicts[dicno];							//TODO need index check 
    	const unsigned long off2 = 16 + dict[off1] * 256 + dict[off1 + 1];	//TODO need index check 
    	const unsigned long blen = dict[off2] * 256 + dict[off2 + 1];		//TODO need index check 
    	const unsigned char* slice = dict + off2 + 2;
		const unsigned long sliceSize = blen & 0x7fff;
    	if (blen & 0x8000) {
        	if (myTargetBufferPtr + sliceSize < myTargetBufferEnd) {
        		std::memcpy(myTargetBufferPtr, slice, sliceSize);
        		myTargetBufferPtr += sliceSize;
        	} else {
        		return;
        	}
    	} else {
			bitsDecompress(BitReader(slice, sliceSize), depth + 1);
		}
	}
}

std::size_t HuffDecompressor::sizeOfTrailingEntries(unsigned char* data, std::size_t size) const {
	std::size_t num = 0;
	std::size_t flags = myExtraFlags >> 1;
	while (flags) {
    	if (flags & 1) {
			if (num < size) {
        		num += readVariableWidthIntegerBE(data, size - num);
			}
		}
    	flags >>= 1;
	}
	return num;
}


std::size_t HuffDecompressor::readVariableWidthIntegerBE(unsigned char* ptr, std::size_t psize) const {
    unsigned char bitsSaved = 0;
	std::size_t result = 0;
    while (true) {
        const unsigned char oneByte = ptr[psize - 1];
        result |= (oneByte & 0x7F) << bitsSaved;
        bitsSaved += 7;
        psize -= 1;
        if (((oneByte & 0x80) != 0) || (bitsSaved >= 28) || (psize == 0)) {
            return result;
		}
	}
}
