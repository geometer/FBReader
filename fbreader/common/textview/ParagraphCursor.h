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

#ifndef __PARAGRAPHCURSOR_H__
#define __PARAGRAPHCURSOR_H__

#include <vector>
#include <string>

#include "../model/TextModel.h"

class TextElement;
class Paragraph;

class WordCursor {

private:
	WordCursor() { myCharNumber = 0; }
	WordCursor(const std::vector<TextElement*>::const_iterator &wordIterator) : myWordIterator(wordIterator) { myCharNumber = 0; }

public:
	const WordCursor& operator = (const WordCursor &cursor) {
		myWordIterator = cursor.myWordIterator;
		myCharNumber = cursor.myCharNumber;
		return *this;
	}
	const WordCursor& operator = (const std::vector<TextElement*>::const_iterator &wordIterator) {
		myWordIterator = wordIterator;
		myCharNumber = 0;
		return *this;
	}
	bool sameElementAs(const WordCursor &cursor) const {
		return myWordIterator == cursor.myWordIterator;
	}
	TextElement &element() const { return **myWordIterator; }
	void nextWord() { myWordIterator++; myCharNumber = 0; }
	void previousWord() { myWordIterator--; myCharNumber = 0; }
	void setCharNumber(int num) { myCharNumber = num; }
	int charNumber() const { return myCharNumber; }

private:
	std::vector<TextElement*>::const_iterator myWordIterator;
	int myCharNumber;

friend class ParagraphCursor;
};

class ParagraphCursor {

private:
	class ParagraphProcessor {

	public:
		ParagraphProcessor(const Paragraph &paragraph, const std::vector<TextMark> &marks, int paragraphNumber, std::vector<TextElement*> &elements);

		void fill();

	private:
		void beforeAddWord();
		void addWord(const std::string &str, int start, int len);

	private:
		const Paragraph &myParagraph;
		std::vector<TextElement*> &myElements;

		std::vector<TextMark>::const_iterator myFirstMark;
		std::vector<TextMark>::const_iterator myLastMark;
		int myWordCounter;
		int myOffset;
	};

protected:
	ParagraphCursor(const TextModel &model);
	ParagraphCursor(const ParagraphCursor &cursor);

public:
	static ParagraphCursor *createCursor(const TextModel &model);
	virtual ParagraphCursor *createCopy() const = 0;
	virtual ~ParagraphCursor();

	virtual bool previous() = 0;
	virtual bool next() = 0;

	bool isStartOfText() const;
	virtual bool isEndOfText() const = 0;
	bool isStartOfParagraph() const;
	bool isEndOfParagraph() const;
	bool isEndOfSection() const;

	int paragraphNumber() const { return myParagraphIterator - myModel.paragraphs().begin(); }
	TextMark position() const;
	int wordNumber(const WordCursor &wi) const { return wi.myWordIterator - myElements.begin(); }
	int wordNumber() const { return wordNumber(myNextElement); }
	int charNumber() const { return myNextElement.myCharNumber; }
	void moveTo(int paragraphNumber, int wordNumber, int charNumber);
	void moveToParagraphStart();
	void moveToParagraphEnd();

	WordCursor wordCursor() const { return myNextElement; }
	void setWordCursor(const WordCursor cursor) { myNextElement = cursor; }
	const WordCursor begin() const { return myElements.begin(); }
	const WordCursor end() const { return myElements.end(); }

private:
	void processControlParagraph(const Paragraph &paragraph);

protected:
	void fill();
	void clear();

private:
	/* assignment is disabled */
	ParagraphCursor &operator = (const ParagraphCursor &);
	
protected:
	const TextModel &myModel;
	std::vector<Paragraph*>::const_iterator myParagraphIterator;
	std::vector<TextElement*> myElements;
	WordCursor myNextElement;
};

class PlainTextParagraphCursor : public ParagraphCursor {

private:
	PlainTextParagraphCursor(const PlainTextParagraphCursor &cursor) : ParagraphCursor(cursor) {}

public:
	PlainTextParagraphCursor(const TextModel &model) : ParagraphCursor(model) {}
	ParagraphCursor *createCopy() const { return new PlainTextParagraphCursor(*this); }

	bool previous();
	bool next();
	bool isEndOfText() const;
};

class TreeParagraphCursor : public ParagraphCursor {

private:
	TreeParagraphCursor(const TreeParagraphCursor &cursor) : ParagraphCursor(cursor) {}

public:
	TreeParagraphCursor(const TreeModel &model) : ParagraphCursor(model) {}
	ParagraphCursor *createCopy() const { return new TreeParagraphCursor(*this); }

	bool previous();
	bool next();
	bool isEndOfText() const;
};

#endif /* __PARAGRAPHCURSOR_H__ */
