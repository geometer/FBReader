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

class Word : public TextElement {

protected:
	Word(int startOffset) { myStartOffset = startOffset; myMark = 0; }

public:
	virtual ~Word() { if (myMark != 0) delete myMark; }
	Kind kind() const { return WORD_ELEMENT; }
	virtual int length() const = 0;
	virtual unsigned short charAt(int i) const = 0;
	virtual std::string utf8String() const = 0;
	int startOffset() const { return myStartOffset; }

	void addMark(int start, int len);

	class WordMark {

	private:
		WordMark(int start, int length) { myStart = start; myLength = length; myNext = 0; }
		void setNext(WordMark *mark) { myNext = mark; }
		~WordMark() { if (myNext != 0) delete myNext; }
		friend class Word;

	public:
		int start() const { return myStart; }
		int length() const { return myLength; }
		const WordMark *next() const { return myNext; }

	private:
		int myStart;
		int myLength;
		WordMark *myNext;
	};

	WordMark *mark() const { return myMark; }

private:
	WordMark *myMark;
	int myStartOffset;
	
private:
	// assignment and copy constructor are disabled
	Word(const Word&);
	Word &operator = (const Word&);
};

class WordBuilder {

public:
	static WordBuilder &instance() { return *ourInstance; }
	static void deleteInstance() { delete ourInstance; }

	virtual Word *buildWord(const char *utf8String, int len, int startOffset) const = 0;

protected:
	virtual ~WordBuilder() {}

protected:
	static WordBuilder *ourInstance;
};

#endif /* __WORD_H__ */
