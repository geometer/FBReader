/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __WORD_H__
#define __WORD_H__

#include <vector>
#include <string>

#include <abstract/ZLPaintContext.h>

#include "TextElement.h"

class Word : public TextElement {

public:
	class WordMark {

	private:
		WordMark(int start, int length);
		void setNext(WordMark *mark);
		~WordMark();
		friend class Word;

	public:
		int start() const;
		int length() const;
		const WordMark *next() const;

	private:
		int myStart;
		int myLength;
		WordMark *myNext;
	};

private:
	Word(const char *data, unsigned short size, size_t paragraphOffset);
	~Word();

public:
	Kind kind() const;
	short width(const ZLPaintContext &context) const;

	void addMark(int start, int len);

	WordMark *mark() const;

	const char *Data;
	const unsigned short Size;
	const unsigned short Length;
	const size_t ParagraphOffset;

private:
	WordMark *myMark;
	mutable short myWidth;
	
private:
	// assignment and copy constructor are disabled
	Word(const Word&);
	Word &operator = (const Word&);

friend class TextElementPool;
};

inline TextElement::Kind Word::kind() const { return WORD_ELEMENT; }
inline Word::WordMark *Word::mark() const { return myMark; }
inline short Word::width(const ZLPaintContext &context) const {
	if (myWidth == -1) {
		myWidth = context.stringWidth(Data, Size);
	}
	return myWidth;
}

inline Word::WordMark::WordMark(int start, int length) { myStart = start; myLength = length; myNext = 0; }
inline void Word::WordMark::setNext(Word::WordMark *mark) { myNext = mark; }
inline int Word::WordMark::start() const { return myStart; }
inline int Word::WordMark::length() const { return myLength; }
inline const Word::WordMark *Word::WordMark::next() const { return myNext; }

#endif /* __WORD_H__ */
