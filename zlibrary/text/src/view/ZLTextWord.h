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

#ifndef __ZLTEXTWORD_H__
#define __ZLTEXTWORD_H__

#include <vector>
#include <string>

#include <ZLPaintContext.h>

#include "ZLTextElement.h"

class ZLTextWord : public ZLTextElement {

public:
	class Mark {

	private:
		Mark(int start, int length);
		void setNext(Mark *mark);
		~Mark();
		friend class ZLTextWord;

	public:
		int start() const;
		int length() const;
		const Mark *next() const;

	private:
		int myStart;
		int myLength;
		Mark *myNext;
	};

private:
	ZLTextWord(const char *data, unsigned short size, size_t paragraphOffset);
	~ZLTextWord();

public:
	Kind kind() const;
	short width(const ZLPaintContext &context) const;

	void addMark(int start, int len);

	Mark *mark() const;

	const char *Data;
	/* size of data in bytes */
	const unsigned short Size;
	/* number of characters */
	const unsigned short Length;
	const size_t ParagraphOffset;

private:
	Mark *myMark;
	mutable short myWidth;
	
private:
	// assignment and copy constructor are disabled
	ZLTextWord(const ZLTextWord&);
	ZLTextWord &operator = (const ZLTextWord&);

friend class ZLTextElementPool;
};

inline ZLTextElement::Kind ZLTextWord::kind() const { return WORD_ELEMENT; }
inline ZLTextWord::Mark *ZLTextWord::mark() const { return myMark; }
inline short ZLTextWord::width(const ZLPaintContext &context) const {
	if (myWidth == -1) {
		myWidth = context.stringWidth(Data, Size);
	}
	return myWidth;
}

inline ZLTextWord::Mark::Mark(int start, int length) { myStart = start; myLength = length; myNext = 0; }
inline void ZLTextWord::Mark::setNext(ZLTextWord::Mark *mark) { myNext = mark; }
inline int ZLTextWord::Mark::start() const { return myStart; }
inline int ZLTextWord::Mark::length() const { return myLength; }
inline const ZLTextWord::Mark *ZLTextWord::Mark::next() const { return myNext; }

#endif /* __ZLTEXTWORD_H__ */
