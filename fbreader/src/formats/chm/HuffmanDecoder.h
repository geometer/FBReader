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

#ifndef __HUFFMANDECODER_H__
#define __HUFFMANDECODER_H__

#include <vector>

#include "BitStream.h"

class HuffmanDecoder {

public:
	HuffmanDecoder();

	bool buildTable();
	void reset();

	unsigned int getSymbol(BitStream &stream) const;

private:
	unsigned char myMaxBitsNumber;
	std::vector<unsigned short> mySymbols;
	std::vector<unsigned char> CodeLengths;
	HuffmanDecoder(const HuffmanDecoder&);
	const HuffmanDecoder &operator = (const HuffmanDecoder&);

friend class LZXDecompressor;
};

inline unsigned int HuffmanDecoder::getSymbol(BitStream &stream) const {
  unsigned int symbol = mySymbols[stream.peek(myMaxBitsNumber)];
  stream.remove(CodeLengths[symbol]);
	return symbol;
}

#endif /* __HUFFMANDECODER_H__ */
