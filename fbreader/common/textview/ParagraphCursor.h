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

#ifndef __PARAGRAPHCURSOR_H__
#define __PARAGRAPHCURSOR_H__

#include <vector>
#include <map>
#include <string>

#include <abstract/shared_ptr.h>
#include <abstract/allocator.h>

#include "../model/TextModel.h"
#include "TextElement.h"
#include "Word.h"

class Paragraph;

class TextElementVector : public std::vector<TextElement*> {

public:
	TextElementVector();
	~TextElementVector();
};

class TextElementPool {

public:
	static TextElementPool Pool;

public:
	TextElementPool();
	~TextElementPool();
	
public:
	TextElement *HSpaceElement;
	TextElement *BeforeParagraphElement;
	TextElement *AfterParagraphElement;
	TextElement *EmptyLineElement;

	Word *getWord(const char *data, unsigned short length, size_t paragraphOffset);
	void storeWord(Word *word);
	ControlElement *getControlElement(shared_ptr<ParagraphEntry> entry);
	void storeControlElement(ControlElement *element);

private:
	Allocator<sizeof(Word),64> myWordAllocator;
	Allocator<sizeof(ControlElement),32> myControlAllocator;
};

class ParagraphCursor {

private:
	class ParagraphProcessor {

	public:
		ParagraphProcessor(const Paragraph &paragraph, const std::vector<TextMark> &marks, int index, TextElementVector &elements);

		void fill();

	private:
		void addWord(const char *ptr, int offset, int len);

	private:
		bool myCheckBreakableCharacters;

	protected:
		const Paragraph &myParagraph;
		TextElementVector &myElements;

		std::vector<TextMark>::const_iterator myFirstMark;
		std::vector<TextMark>::const_iterator myLastMark;
		int myOffset;
	};

protected:
	ParagraphCursor(const TextModel &model, size_t index);
	virtual ParagraphCursor *createCursor(size_t index) const = 0;

public:
	static ParagraphCursor *createCursor(const TextModel &model);
	virtual ~ParagraphCursor();

	bool isFirst() const;
	virtual bool isLast() const = 0;
	bool isEndOfSection() const;

	size_t paragraphLength() const;
	size_t index() const;

	virtual shared_ptr<ParagraphCursor> previous() const = 0;
	virtual shared_ptr<ParagraphCursor> next() const = 0;
	shared_ptr<ParagraphCursor> cursor(size_t index) const;

	const TextElement &operator [] (size_t index) const;
	const Paragraph &paragraph() const;

private:
	void processControlParagraph(const Paragraph &paragraph);

protected:
	void fill();
	void clear();

private:
	/* copy constructor & assignment are disabled */
	ParagraphCursor(const ParagraphCursor&);
	ParagraphCursor &operator = (const ParagraphCursor&);
	
protected:
	const TextModel &myModel;
	size_t myIndex;
	TextElementVector myElements;

friend class WordCursor;
};

class ParagraphCursorCache {

public:
	static void put(const Paragraph *paragraph, shared_ptr<ParagraphCursor> cursor);
	static shared_ptr<ParagraphCursor> get(const Paragraph *paragraph);

	static void clear();
	static void cleanup();

private:
	static std::map<const Paragraph*, weak_ptr<ParagraphCursor> > ourCache;

private:
	// instance creation is disabled
	ParagraphCursorCache();
};

class WordCursor {

public:
	WordCursor();
	WordCursor(const WordCursor &cursor);
	const WordCursor &operator = (const WordCursor &cursor);
	const WordCursor &operator = (ParagraphCursor *paragraphCursor);
	~WordCursor();

	bool isNull() const;
	bool sameElementAs(const WordCursor &cursor) const;
	bool operator == (const WordCursor &cursor) const;
	bool operator != (const WordCursor &cursor) const;
	bool operator < (const WordCursor &cursor) const;
	bool isStartOfParagraph() const;
	bool isEndOfParagraph() const;
	unsigned int wordNumber() const;
	unsigned int charNumber() const;
	const TextElement &element() const;
	TextMark position() const;
	const ParagraphCursor &paragraphCursor() const;

	void nextWord();
	void previousWord();
	bool nextParagraph();
	bool previousParagraph();
	void moveToParagraphStart();
	void moveToParagraphEnd();
	void moveToParagraph(int paragraphNumber);
	void moveTo(int wordNumber, int charNumber);
	void setCharNumber(int charNumber);

	void rebuild();

private:
	shared_ptr<ParagraphCursor> myParagraphCursor;
	unsigned int myWordNumber;
	unsigned int myCharNumber;
};

class PlainTextParagraphCursor : public ParagraphCursor {

private:
	PlainTextParagraphCursor(const TextModel &model, size_t index);

protected:
	ParagraphCursor *createCursor(size_t index) const;

public:
	~PlainTextParagraphCursor();

	shared_ptr<ParagraphCursor> previous() const;
	shared_ptr<ParagraphCursor> next() const;
	bool isLast() const;

friend class ParagraphCursor;
};

class TreeParagraphCursor : public ParagraphCursor {

private:
	TreeParagraphCursor(const TreeModel &model, size_t index);

protected:
	ParagraphCursor *createCursor(size_t index) const;

public:
	~TreeParagraphCursor();

	shared_ptr<ParagraphCursor> previous() const;
	shared_ptr<ParagraphCursor> next() const;
	bool isLast() const;

friend class ParagraphCursor;
};

inline TextElementVector::TextElementVector() {}

inline Word *TextElementPool::getWord(const char *data, unsigned short length, size_t paragraphOffset) {
	return new (myWordAllocator.allocate()) Word(data, length, paragraphOffset);
}
inline void TextElementPool::storeWord(Word *word) {
	word->~Word();
	myWordAllocator.free((void*)word);
}
inline ControlElement *TextElementPool::getControlElement(shared_ptr<ParagraphEntry> entry) {
	return new (myControlAllocator.allocate()) ControlElement(entry);
}
inline void TextElementPool::storeControlElement(ControlElement *element) {
	element->~ControlElement();
	myControlAllocator.free((void*)element);
}

inline size_t ParagraphCursor::index() const { return myIndex; }
inline const TextElement &ParagraphCursor::operator [] (size_t index) const { return *myElements[index]; }
inline const Paragraph &ParagraphCursor::paragraph() const { return *myModel[myIndex]; }
inline size_t ParagraphCursor::paragraphLength() const { return myElements.size(); }

inline WordCursor::WordCursor() : myWordNumber(0), myCharNumber(0) {}
inline WordCursor::WordCursor(const WordCursor &cursor) : myParagraphCursor(cursor.myParagraphCursor), myWordNumber(cursor.myWordNumber), myCharNumber(cursor.myCharNumber) {}
inline const WordCursor &WordCursor::operator = (const WordCursor &cursor) {
	myParagraphCursor = cursor.myParagraphCursor;
	myWordNumber = cursor.myWordNumber;
	myCharNumber = cursor.myCharNumber;
	return *this;
}
inline WordCursor::~WordCursor() {}

inline bool WordCursor::isNull() const { return myParagraphCursor.isNull(); }
inline bool WordCursor::sameElementAs(const WordCursor &cursor) const {
	return myWordNumber == cursor.myWordNumber;
}
inline bool WordCursor::operator == (const WordCursor &cursor) const {
	return
		(myParagraphCursor->index() == cursor.myParagraphCursor->index()) &&
		(myWordNumber == cursor.myWordNumber) &&
		(myCharNumber == cursor.myCharNumber);
}
inline bool WordCursor::operator != (const WordCursor &cursor) const {
	return !operator == (cursor);
}
inline const TextElement &WordCursor::element() const { return (*myParagraphCursor)[myWordNumber]; }
inline bool WordCursor::isStartOfParagraph() const {
	return (myWordNumber == 0) && (myCharNumber == 0);
}
inline bool WordCursor::isEndOfParagraph() const {
	return myWordNumber == myParagraphCursor->paragraphLength();
}
inline unsigned int WordCursor::wordNumber() const { return myWordNumber; }
inline unsigned int WordCursor::charNumber() const { return myCharNumber; }
inline const ParagraphCursor &WordCursor::paragraphCursor() const { return *myParagraphCursor; }

inline void WordCursor::nextWord() { ++myWordNumber; myCharNumber = 0; }
inline void WordCursor::previousWord() { --myWordNumber; myCharNumber = 0; }

inline PlainTextParagraphCursor::PlainTextParagraphCursor(const TextModel &model, size_t index) : ParagraphCursor(model, index) {}
inline PlainTextParagraphCursor::~PlainTextParagraphCursor() {}
inline ParagraphCursor *PlainTextParagraphCursor::createCursor(size_t index) const { return new PlainTextParagraphCursor(myModel, index); }

inline TreeParagraphCursor::TreeParagraphCursor(const TreeModel &model, size_t index) : ParagraphCursor(model, index) {}
inline TreeParagraphCursor::~TreeParagraphCursor() {}
inline ParagraphCursor *TreeParagraphCursor::createCursor(size_t index) const { return new TreeParagraphCursor((const TreeModel&)myModel, index); }

#endif /* __PARAGRAPHCURSOR_H__ */
