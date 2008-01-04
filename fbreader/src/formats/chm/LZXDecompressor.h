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

#ifndef __LZXDECOMPRESSOR_H__
#define __LZXDECOMPRESSOR_H__

#include <string>
#include <vector>

#include "BitStream.h"
#include "HuffmanDecoder.h"

class LZXDecompressor {

public:
	LZXDecompressor(int windowSizeIndex);
	void reset();

	bool decompress(const std::string &data, unsigned char *outBuffer, const size_t outSize);

private:
	struct DecodingState {
		std::vector<unsigned char>::iterator WindowIterator;
		unsigned int R0;
		unsigned int R1;
		unsigned int R2;
	};

	bool readLengths(HuffmanDecoder &decoder, size_t from, size_t size);
	bool decodeBytes(DecodingState &state, size_t bytesToDecode);

private:
	enum BlockType {
		UNKNOWNN = 0,
		VERBATIM = 1,
		ALIGNED = 2,
		UNCOMPRESSED = 3
	};

	BlockType myCurrentBlockType;
	bool myReadHeader;

	std::vector<unsigned char> myWindow;

	DecodingState myState;

	size_t myBlockSize;
	size_t myBlockBytesLeft;

	const unsigned int mySlotNumber;
	HuffmanDecoder myMainTree;
	HuffmanDecoder myLengthTree;
	HuffmanDecoder myAlignedOffsetTree;

	BitStream myBitStream;

	class E8Decoder {

	public:
		void reset(unsigned int fileSize);
		void decode(unsigned char *buffer, const size_t size);

	private:
		unsigned int myFramesCounter;
		unsigned int myFileSize;
		unsigned int myPosition;
	};

	E8Decoder myE8Decoder;
};

#endif /* __LZXDECOMPRESSOR_H__ */
