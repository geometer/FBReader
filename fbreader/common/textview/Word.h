/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __WORD_H__
#define __WORD_H__

#include <vector>
#include <string>

#include "TextElement.h"
#include "../view/PaintContext.h"

class Word : public TextElement {

public:
	class WordMark {

	private:
		WordMark(int start, int length) VIEW_SECTION;
		void setNext(WordMark *mark) VIEW_SECTION;
		~WordMark() VIEW_SECTION;
		friend class Word;

	public:
		int start() const VIEW_SECTION;
		int length() const VIEW_SECTION;
		const WordMark *next() const VIEW_SECTION;

	private:
		int myStart;
		int myLength;
		WordMark *myNext;
	};

public:
	Word(const std::string &utf8String, size_t start, size_t length, size_t paragraphOffset) VIEW_SECTION;

public:
	~Word() VIEW_SECTION;
	Kind kind() const VIEW_SECTION;
	const char *data() const VIEW_SECTION;
	size_t size() const VIEW_SECTION;
	size_t length() const VIEW_SECTION;
	size_t paragraphOffset() const VIEW_SECTION;
	int width(const PaintContext &context) const VIEW_SECTION;

	void addMark(int start, int len) VIEW_SECTION;

	WordMark *mark() const VIEW_SECTION;

private:
	const std::string &myData;
	size_t myStart;
	size_t mySize;
	size_t myLength;

	WordMark *myMark;
	size_t myParagraphOffset;

	mutable int myWidth;
	
private:
	// assignment and copy constructor are disabled
	Word(const Word&) VIEW_SECTION;
	Word &operator = (const Word&) VIEW_SECTION;
};

inline Word::~Word() { if (myMark != 0) delete myMark; }
inline TextElement::Kind Word::kind() const { return WORD_ELEMENT; }
inline const char *Word::data() const { return myData.data() + myStart; }
inline size_t Word::size() const { return mySize; }
inline size_t Word::length() const { return myLength; }
inline size_t Word::paragraphOffset() const { return myParagraphOffset; }
inline Word::WordMark *Word::mark() const { return myMark; }
inline int Word::width(const PaintContext &context) const {
	if (myWidth == -1) {
		myWidth = context.stringWidth(data(), mySize);
	}
	return myWidth;
}

inline Word::WordMark::WordMark(int start, int length) { myStart = start; myLength = length; myNext = 0; }
inline Word::WordMark::~WordMark() { if (myNext != 0) delete myNext; }
inline void Word::WordMark::setNext(Word::WordMark *mark) { myNext = mark; }
inline int Word::WordMark::start() const { return myStart; }
inline int Word::WordMark::length() const { return myLength; }
inline const Word::WordMark *Word::WordMark::next() const { return myNext; }

#endif /* __WORD_H__ */
