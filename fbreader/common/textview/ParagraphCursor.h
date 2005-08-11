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
#include <map>
#include <string>

#include <abstract/shared_ptr.h>
#include <abstract/allocator.h>

#include "../model/TextModel.h"
#include "Word.h"

class TextElement;
class Paragraph;

class TextElementVector : public std::vector<TextElement*> {

public:
	TextElementVector() VIEW_SECTION;
	~TextElementVector() VIEW_SECTION;
};

class TextElementPool {

public:
	static TextElementPool Pool;

public:
	TextElementPool() VIEW_SECTION;
	~TextElementPool() VIEW_SECTION;
	
public:
	TextElement *HSpaceElement;
	TextElement *BeforeParagraphElement;
	TextElement *AfterParagraphElement;
	TextElement *IndentElement;
	TextElement *EmptyLineElement;

	Word *getWord(const char *data, unsigned short length, size_t paragraphOffset) VIEW_SECTION;
	void storeWord(Word *word) VIEW_SECTION;
	ControlElement *getControlElement(const ControlEntry &entry) VIEW_SECTION;
	void storeControlElement(ControlElement *element) VIEW_SECTION;

private:
	Allocator<sizeof(Word),64> myWordAllocator;
	Allocator<sizeof(ControlElement),32> myControlAllocator;
};

class ParagraphCursor {

public:
	class Cache {

	public:
		Cache() VIEW_SECTION;
		~Cache() VIEW_SECTION;

	private:
		static std::map<Paragraph*,shared_ptr<TextElementVector> > ourCache;
		static int ourCacheCounter;

	friend class ParagraphCursor;
	};
	
private:
	class ParagraphProcessor {

	public:
		ParagraphProcessor(const Paragraph &paragraph, const std::vector<TextMark> &marks, int paragraphNumber, const shared_ptr<TextElementVector> elements) VIEW_SECTION;
		~ParagraphProcessor() VIEW_SECTION;

		void fill() VIEW_SECTION;

	private:
		void beforeAddWord() VIEW_SECTION;
		void addWord(const char *ptr, int offset, int len) VIEW_SECTION;

	private:
		const Paragraph &myParagraph;
		shared_ptr<TextElementVector> myElements;

		std::vector<TextMark>::const_iterator myFirstMark;
		std::vector<TextMark>::const_iterator myLastMark;
		int myWordCounter;
		int myOffset;
	};

protected:
	ParagraphCursor(const TextModel &model) VIEW_SECTION;
	ParagraphCursor(const ParagraphCursor &cursor) VIEW_SECTION;
	virtual ParagraphCursor *createCopy() const VIEW_SECTION = 0;

public:
	static ParagraphCursor *createCursor(const TextModel &model) VIEW_SECTION;
	virtual ~ParagraphCursor() VIEW_SECTION;

	virtual bool previous() VIEW_SECTION = 0;
	virtual bool next() VIEW_SECTION = 0;

	bool isFirst() const VIEW_SECTION;
	virtual bool isLast() const VIEW_SECTION = 0;
	bool isEndOfSection() const VIEW_SECTION;

	int paragraphLength() const VIEW_SECTION;
	int paragraphNumber() const VIEW_SECTION;
	void moveTo(int paragraphNumber) VIEW_SECTION;

private:
	void processControlParagraph(const Paragraph &paragraph) VIEW_SECTION;

protected:
	void fill() VIEW_SECTION;
	void clear() VIEW_SECTION;

private:
	/* assignment is disabled */
	ParagraphCursor &operator = (const ParagraphCursor &) VIEW_SECTION;
	
protected:
	const TextModel &myModel;
	std::vector<Paragraph*>::const_iterator myParagraphIterator;
	shared_ptr<TextElementVector> myElements;

friend class WordCursor;
};

class WordCursor {

public:
	WordCursor() VIEW_SECTION;
	const WordCursor &operator = (ParagraphCursor *paragraphCursor) VIEW_SECTION;
	~WordCursor() VIEW_SECTION;

	bool isNull() const VIEW_SECTION;
	bool sameElementAs(const WordCursor &cursor) const VIEW_SECTION;
	bool isStartOfParagraph() const VIEW_SECTION;
	bool isEndOfParagraph() const VIEW_SECTION;
	int wordNumber() const VIEW_SECTION;
	int charNumber() const VIEW_SECTION;
	TextElement &element() const VIEW_SECTION;
	TextMark position() const VIEW_SECTION;
	const ParagraphCursor &paragraphCursor() const VIEW_SECTION;

	void nextWord() VIEW_SECTION;
	void previousWord() VIEW_SECTION;
	bool nextParagraph() VIEW_SECTION;
	bool previousParagraph() VIEW_SECTION;
	void moveToParagraphStart() VIEW_SECTION;
	void moveToParagraphEnd() VIEW_SECTION;
	void moveToParagraph(int paragraphNumber) VIEW_SECTION;
	void moveTo(int wordNumber, int charNumber) VIEW_SECTION;
	void setCharNumber(int charNumber) VIEW_SECTION;

	void rebuild() VIEW_SECTION;

private:
	shared_ptr<ParagraphCursor> myParagraphCursor;
	unsigned int myWordNumber;
	unsigned int myCharNumber;
};

class PlainTextParagraphCursor : public ParagraphCursor {

private:
	PlainTextParagraphCursor(const PlainTextParagraphCursor &cursor) VIEW_SECTION;

protected:
	ParagraphCursor *createCopy() const VIEW_SECTION;

public:
	PlainTextParagraphCursor(const TextModel &model) VIEW_SECTION;
	~PlainTextParagraphCursor() VIEW_SECTION;

	bool previous() VIEW_SECTION;
	bool next() VIEW_SECTION;
	bool isLast() const VIEW_SECTION;
};

class TreeParagraphCursor : public ParagraphCursor {

private:
	TreeParagraphCursor(const TreeParagraphCursor &cursor) VIEW_SECTION;

protected:
	ParagraphCursor *createCopy() const VIEW_SECTION;

public:
	TreeParagraphCursor(const TreeModel &model) VIEW_SECTION;
	~TreeParagraphCursor() VIEW_SECTION;

	bool previous() VIEW_SECTION;
	bool next() VIEW_SECTION;
	bool isLast() const VIEW_SECTION;
};

inline TextElementVector::TextElementVector() {}

inline Word *TextElementPool::getWord(const char *data, unsigned short length, size_t paragraphOffset) {
	return new (myWordAllocator.allocate()) Word(data, length, paragraphOffset);
}
inline void TextElementPool::storeWord(Word *word) {
	word->~Word();
	myWordAllocator.free((void*)word);
}
inline ControlElement *TextElementPool::getControlElement(const ControlEntry &entry) {
	return new (myControlAllocator.allocate()) ControlElement(entry);
}
inline void TextElementPool::storeControlElement(ControlElement *element) {
	element->~ControlElement();
	myControlAllocator.free((void*)element);
}

inline WordCursor::WordCursor() : myCharNumber(0) {}
inline WordCursor::~WordCursor() {}

inline bool WordCursor::isNull() const { return myParagraphCursor.isNull(); }
inline bool WordCursor::sameElementAs(const WordCursor &cursor) const {
	return (myParagraphCursor == cursor.myParagraphCursor) && (myWordNumber == cursor.myWordNumber);
}
inline TextElement &WordCursor::element() const { return *(*myParagraphCursor->myElements)[myWordNumber]; }
inline bool WordCursor::isStartOfParagraph() const {
	return (myWordNumber == 0) && (myCharNumber == 0);
}
inline bool WordCursor::isEndOfParagraph() const {
	return myWordNumber == myParagraphCursor->myElements->size();
}
inline int WordCursor::wordNumber() const { return myWordNumber; }
inline int WordCursor::charNumber() const { return myCharNumber; }
inline const ParagraphCursor &WordCursor::paragraphCursor() const { return *myParagraphCursor; }

inline void WordCursor::nextWord() { myWordNumber++; myCharNumber = 0; }
inline void WordCursor::previousWord() { myWordNumber--; myCharNumber = 0; }

inline ParagraphCursor::Cache::Cache() { ourCacheCounter++; }
inline ParagraphCursor::Cache::~Cache() {
	ourCacheCounter--;
	if (ourCacheCounter == 0) {
		ourCache.clear();
	}
}

inline int ParagraphCursor::paragraphLength() const { return myElements->size(); }
inline int ParagraphCursor::paragraphNumber() const { return myParagraphIterator - myModel.paragraphs().begin(); }

inline PlainTextParagraphCursor::PlainTextParagraphCursor(const PlainTextParagraphCursor &cursor) : ParagraphCursor(cursor) {}
inline PlainTextParagraphCursor::PlainTextParagraphCursor(const TextModel &model) : ParagraphCursor(model) {}
inline PlainTextParagraphCursor::~PlainTextParagraphCursor() {}
inline ParagraphCursor *PlainTextParagraphCursor::createCopy() const { return new PlainTextParagraphCursor(*this); }

inline TreeParagraphCursor::TreeParagraphCursor(const TreeParagraphCursor &cursor) : ParagraphCursor(cursor) {}
inline TreeParagraphCursor::TreeParagraphCursor(const TreeModel &model) : ParagraphCursor(model) {}
inline TreeParagraphCursor::~TreeParagraphCursor() {}
inline ParagraphCursor *TreeParagraphCursor::createCopy() const { return new TreeParagraphCursor(*this); }

#endif /* __PARAGRAPHCURSOR_H__ */
