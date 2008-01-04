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

#include <string.h>

#include "LZXDecompressor.h"

static unsigned int slotNumber(int windowSizeIndex) {
	if (windowSizeIndex == 20) {
		return 42;
	} else if (windowSizeIndex == 21) {
		return 50;
	} else {
		return 2 * windowSizeIndex;
	}
}

LZXDecompressor::LZXDecompressor(int windowSizeIndex) : myWindow(1 << windowSizeIndex, 0), mySlotNumber(slotNumber(windowSizeIndex)) {
	reset();
}

void LZXDecompressor::reset() {
	myCurrentBlockType = UNKNOWNN;
	myReadHeader = true;

	myState.WindowIterator = myWindow.begin();
	myState.R0 = 1;
	myState.R1 = 1;
	myState.R2 = 1;

	myMainTree.reset();
	myLengthTree.reset();

	myBlockBytesLeft = 0;

	myE8Decoder.reset(0);
}

static bool fill(std::vector<unsigned char> &data, std::vector<unsigned char>::iterator &it, int num, unsigned char value) {
	if (data.end() - it < num) {
		return false;
	}
	std::vector<unsigned char>::iterator end = it + num;
	while (it != end) {
		*it++ = value;
	}
	return true;
}

bool LZXDecompressor::readLengths(HuffmanDecoder &decoder, size_t from, size_t size) {
	HuffmanDecoder preTree;
	preTree.CodeLengths.reserve(20);
	for (int i = 0; i < 20; i++) {
		preTree.CodeLengths.push_back(myBitStream.get(4));
	}
	if (!preTree.buildTable()) {
		return false;
	}

	std::vector<unsigned char> &lengths = decoder.CodeLengths;
	if (lengths.size() < from + size) {
		lengths.insert(lengths.end(), from + size - lengths.size(), 0);
	}
	std::vector<unsigned char>::iterator start = lengths.begin() + from;
	std::vector<unsigned char>::iterator end = start + size;
	for (std::vector<unsigned char>::iterator it = start; it != end; ) {
		int z = preTree.getSymbol(myBitStream);
		if (z == 17) {
			if (!fill(lengths, it, myBitStream.get(4) + 4, 0)) {
				return false;
			}
		} else if (z == 18) {
			if (!fill(lengths, it, myBitStream.get(5) + 20, 0)) {
				return false;
			}
		} else if (z == 19) {
			unsigned int num = myBitStream.get(1) + 4;
			z = *it - preTree.getSymbol(myBitStream);
			if (!fill(lengths, it, num, (z < 0) ? z + 17 : z)) {
				return false;
			}
		} else {
			z = *it - z;
			*it++ = (z < 0) ? z + 17 : z;
		}
	}

	return true;
}

static const unsigned int basePosition[51] = {
	      0,       1,       2,       3,       4,       6,       8,      12,
	     16,      24,      32,      48,      64,      96,     128,     192,
	    256,     384,     512,     768,    1024,    1536,    2048,    3072,
	   4096,    6144,    8192,   12288,   16384,   24576,   32768,   49152,
	  65536,   98304,  131072,  196608,  262144,  393216,  524288,  655360,
	 786432,  917504, 1048576, 1179648, 1310720, 1441792, 1572864, 1703936,
	1835008, 1966080, 2097152
};

bool LZXDecompressor::decodeBytes(DecodingState &state, size_t bytesToDecode) {
	if (myCurrentBlockType == UNCOMPRESSED) {
		if (!myBitStream.getBytesDirect(&*state.WindowIterator, bytesToDecode)) {
			return false;
		}
		state.WindowIterator += bytesToDecode;
		return true;
	}

	while (bytesToDecode > 0) {
		int symbol = myMainTree.getSymbol(myBitStream);
		if (symbol < 256) {
			*state.WindowIterator++ = symbol;
			--bytesToDecode;
			continue;
		}

		size_t length = symbol % 8;
		if (length == 7) {
			length += myLengthTree.getSymbol(myBitStream);
		}
		length += 2;
		if (length > bytesToDecode) {
			return false;
		}

		size_t offset = (symbol - 256) / 8;
		switch (offset) {
			case 0:
				offset = state.R0;
				break;
			case 1:
				offset = state.R1;
				state.R1 = state.R0;
				state.R0 = offset;
				break;
			case 2:
				offset = state.R2;
				state.R2 = state.R0;
				state.R0 = offset;
				break;
			default:
				if ((myCurrentBlockType == VERBATIM) && (offset == 3)) {
					offset = 1;
				} else {
					if (offset > 50) {
						return false;
					}
					const int positionFooterBits = std::max(0, std::min((int)offset / 2 - 1, 17));
					offset = basePosition[offset] - 2;
					if ((myCurrentBlockType == VERBATIM) || (positionFooterBits == 1) || (positionFooterBits == 2)) {
						offset += myBitStream.get(positionFooterBits);
					} else if (positionFooterBits == 3) {
						offset += myAlignedOffsetTree.getSymbol(myBitStream);
					} else if (positionFooterBits > 3) {
						offset += 8 * myBitStream.get(positionFooterBits - 3);
						offset += myAlignedOffsetTree.getSymbol(myBitStream);
					} else {
						offset = 1;
					}
				}
				state.R2 = state.R1;
				state.R1 = state.R0;
				state.R0 = offset;
				break;
		}

		std::vector<unsigned char>::iterator srcIt = state.WindowIterator - offset + myWindow.size();
		if (srcIt < myWindow.begin()) {
			return false;
		}
		for (size_t i = 0; i < length; ++i) {
			if (srcIt >= myWindow.end()) {
				srcIt -= myWindow.size();
				if (srcIt >= myWindow.end()) {
					return false;
				}
			}
			*state.WindowIterator++ = *srcIt++;
		}
		bytesToDecode -= length;
	}
	return true;
}

bool LZXDecompressor::decompress(const std::string &data, unsigned char *outBuffer, const size_t outSize) {
	myBitStream.setData(data);

	if (myReadHeader) {
		if (myBitStream.get(1) == 1) {
			myE8Decoder.reset(myBitStream.get(32));
		}
		myReadHeader = false;
	}

	DecodingState state = myState;

	for (size_t bytesToWrite = outSize; bytesToWrite > 0; ) {
		if (myBlockBytesLeft == 0) {
			if (myCurrentBlockType == UNCOMPRESSED) {
				if (myBlockSize & 1) {
					myBitStream.remove(8);
				}
				myBitStream.reset();
			}

			myCurrentBlockType = (BlockType)myBitStream.get(3);
			myBlockSize = myBitStream.get(24);
			myBlockBytesLeft = myBlockSize;

			switch (myCurrentBlockType) {
				case UNCOMPRESSED:
					myBitStream.reset();
					state.R0 = myBitStream.get4BytesDirect();
					state.R1 = myBitStream.get4BytesDirect();
					state.R2 = myBitStream.get4BytesDirect();
					break;

				case ALIGNED:
					myAlignedOffsetTree.CodeLengths.clear();
					for (int i = 0; i < 8; i++) {
						myAlignedOffsetTree.CodeLengths.push_back(myBitStream.get(3));
					}
					if (!myAlignedOffsetTree.buildTable()) {
						return false;
					}
					// no break; it's not a mistake

				case VERBATIM:
					if (!readLengths(myMainTree, 0, 256) ||
							!readLengths(myMainTree, 256, 8 * mySlotNumber) ||
							!readLengths(myLengthTree, 0, 249) ||
							!myMainTree.buildTable() ||
							!myLengthTree.buildTable()) {
						return false;
					}
					break;

				default:
					return false;
			}
		}

		while ((myBlockBytesLeft > 0) && (bytesToWrite > 0)) {
			size_t bytesToDecode = std::min(myBlockBytesLeft, bytesToWrite);
			if (state.WindowIterator + bytesToDecode > myWindow.end()) {
				return false;
			}

			if (!decodeBytes(state, bytesToDecode)) {
				return false;
			}

			bytesToWrite -= bytesToDecode;
			myBlockBytesLeft -= bytesToDecode;
		}
	}

	std::vector<unsigned char>::iterator jt =
		(state.WindowIterator != myWindow.begin()) ? state.WindowIterator : myWindow.end();
	memcpy(outBuffer, &*(jt - outSize), outSize);

	myState = state;

	myE8Decoder.decode(outBuffer, outSize);

	return true;
}
