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

#ifndef __ZLTEXTPARAGRAPHCURSOR_H__
#define __ZLTEXTPARAGRAPHCURSOR_H__

#include <vector>
#include <map>
#include <string>

#include <shared_ptr.h>
#include <allocator.h>

#include <ZLTextModel.h>

#include "ZLTextElement.h"
#include "ZLTextWord.h"

class ZLTextParagraph;

class ZLTextElementVector : public std::vector<ZLTextElement*> {

public:
	ZLTextElementVector();
	~ZLTextElementVector();
};

class ZLTextElementPool {

public:
	static ZLTextElementPool Pool;

public:
	ZLTextElementPool();
	~ZLTextElementPool();
	
public:
	ZLTextElement *HSpaceElement;
	ZLTextElement *NBHSpaceElement;
	ZLTextElement *BeforeParagraphElement;
	ZLTextElement *AfterParagraphElement;
	ZLTextElement *EmptyLineElement;

	ZLTextWord *getWord(const char *data, unsigned short length, size_t paragraphOffset);
	void storeWord(ZLTextWord *word);
	ZLTextControlElement *getControlElement(shared_ptr<ZLTextParagraphEntry> entry);
	void storeControlElement(ZLTextControlElement *element);

private:
	Allocator<sizeof(ZLTextWord),64> myWordAllocator;
	Allocator<sizeof(ZLTextControlElement),32> myControlAllocator;
};

class ZLTextParagraphCursor;
typedef shared_ptr<ZLTextParagraphCursor> ZLTextParagraphCursorPtr;

class ZLTextParagraphCursor {

private:
	class Processor {

	private:
		static bool ourIndexIsInitialised;

	public:
		Processor(const std::string &language, const ZLTextParagraph &paragraph, const std::vector<ZLTextMark> &marks, int index, ZLTextElementVector &elements);
		virtual ~Processor();
		void fill();

	private:
		void processTextEntry(const ZLTextEntry &textEntry);
		void addWord(const char *ptr, int offset, int len);

	private:
		const ZLTextParagraph &myParagraph;
		ZLTextElementVector &myElements;

		std::vector<ZLTextMark>::const_iterator myFirstMark;
		std::vector<ZLTextMark>::const_iterator myLastMark;
		int myOffset;

		const std::string myLanguage;
		char *myBreaksTable;
		size_t myBreaksTableLength;
	};

protected:
	ZLTextParagraphCursor(const ZLTextModel &model, size_t index);

public:
	static ZLTextParagraphCursorPtr cursor(const ZLTextModel &model, size_t index = 0);
	virtual ~ZLTextParagraphCursor();

	bool isFirst() const;
	virtual bool isLast() const = 0;
	bool isEndOfSection() const;

	size_t paragraphLength() const;
	size_t index() const;

	virtual ZLTextParagraphCursorPtr previous() const = 0;
	virtual ZLTextParagraphCursorPtr next() const = 0;

	const ZLTextElement &operator [] (size_t index) const;
	const ZLTextParagraph &paragraph() const;

private:
	void processControlParagraph(const ZLTextParagraph &paragraph);

protected:
	void fill();
	void clear();

private:
	/* copy constructor & assignment are disabled */
	ZLTextParagraphCursor(const ZLTextParagraphCursor&);
	ZLTextParagraphCursor &operator = (const ZLTextParagraphCursor&);
	
protected:
	const ZLTextModel &myModel;
	size_t myIndex;
	ZLTextElementVector myElements;

friend class ZLTextWordCursor;
};

class ZLTextParagraphCursorCache {

public:
	static void put(const ZLTextParagraph *paragraph, ZLTextParagraphCursorPtr cursor);
	static ZLTextParagraphCursorPtr get(const ZLTextParagraph *paragraph);

	static void clear();
	static void cleanup();

private:
	static std::map<const ZLTextParagraph*, weak_ptr<ZLTextParagraphCursor> > ourCache;
	static ZLTextParagraphCursorPtr ourLastAdded;

private:
	// instance creation is disabled
	ZLTextParagraphCursorCache();
};

class ZLTextWordCursor {

public:
	ZLTextWordCursor();
	ZLTextWordCursor(const ZLTextWordCursor &cursor);
	const ZLTextWordCursor &operator = (const ZLTextWordCursor &cursor);
	const ZLTextWordCursor &operator = (ZLTextParagraphCursorPtr paragraphCursor);

	bool isNull() const;
	bool equalWordNumber(const ZLTextWordCursor &cursor) const;
	bool sameElementAs(const ZLTextWordCursor &cursor) const;
	bool operator == (const ZLTextWordCursor &cursor) const;
	bool operator != (const ZLTextWordCursor &cursor) const;
	bool operator < (const ZLTextWordCursor &cursor) const;
	bool isStartOfParagraph() const;
	bool isEndOfParagraph() const;
	unsigned int wordNumber() const;
	unsigned int charNumber() const;
	const ZLTextElement &element() const;
	ZLTextMark position() const;
	ZLTextParagraphCursorPtr paragraphCursorPtr() const;
	const ZLTextParagraphCursor &paragraphCursor() const;

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
	ZLTextParagraphCursorPtr myParagraphCursor;
	unsigned int myWordNumber;
	unsigned int myCharNumber;
};

class ZLTextPlainParagraphCursor : public ZLTextParagraphCursor {

private:
	ZLTextPlainParagraphCursor(const ZLTextModel &model, size_t index);

public:
	~ZLTextPlainParagraphCursor();

	ZLTextParagraphCursorPtr previous() const;
	ZLTextParagraphCursorPtr next() const;
	bool isLast() const;

friend class ZLTextParagraphCursor;
};

class ZLTextTreeParagraphCursor : public ZLTextParagraphCursor {

private:
	ZLTextTreeParagraphCursor(const ZLTextTreeModel &model, size_t index);

public:
	~ZLTextTreeParagraphCursor();

	ZLTextParagraphCursorPtr previous() const;
	ZLTextParagraphCursorPtr next() const;
	bool isLast() const;

friend class ZLTextParagraphCursor;
};

inline ZLTextElementVector::ZLTextElementVector() {}

inline ZLTextWord *ZLTextElementPool::getWord(const char *data, unsigned short length, size_t paragraphOffset) {
	return new (myWordAllocator.allocate()) ZLTextWord(data, length, paragraphOffset);
}
inline void ZLTextElementPool::storeWord(ZLTextWord *word) {
	word->~ZLTextWord();
	myWordAllocator.free((void*)word);
}
inline ZLTextControlElement *ZLTextElementPool::getControlElement(shared_ptr<ZLTextParagraphEntry> entry) {
	return new (myControlAllocator.allocate()) ZLTextControlElement(entry);
}
inline void ZLTextElementPool::storeControlElement(ZLTextControlElement *element) {
	element->~ZLTextControlElement();
	myControlAllocator.free((void*)element);
}

inline size_t ZLTextParagraphCursor::index() const { return myIndex; }
inline const ZLTextElement &ZLTextParagraphCursor::operator [] (size_t index) const { return *myElements[index]; }
inline const ZLTextParagraph &ZLTextParagraphCursor::paragraph() const { return *myModel[myIndex]; }
inline size_t ZLTextParagraphCursor::paragraphLength() const { return myElements.size(); }

inline ZLTextWordCursor::ZLTextWordCursor() : myWordNumber(0), myCharNumber(0) {}
inline ZLTextWordCursor::ZLTextWordCursor(const ZLTextWordCursor &cursor) : myParagraphCursor(cursor.myParagraphCursor), myWordNumber(cursor.myWordNumber), myCharNumber(cursor.myCharNumber) {}
inline const ZLTextWordCursor &ZLTextWordCursor::operator = (const ZLTextWordCursor &cursor) {
	myParagraphCursor = cursor.myParagraphCursor;
	myWordNumber = cursor.myWordNumber;
	myCharNumber = cursor.myCharNumber;
	return *this;
}

inline bool ZLTextWordCursor::isNull() const { return myParagraphCursor.isNull(); }
inline bool ZLTextWordCursor::equalWordNumber(const ZLTextWordCursor &cursor) const {
	return myWordNumber == cursor.myWordNumber;
}
inline bool ZLTextWordCursor::sameElementAs(const ZLTextWordCursor &cursor) const {
	return (myWordNumber == cursor.myWordNumber) && (myParagraphCursor->index() == cursor.myParagraphCursor->index());
}
inline bool ZLTextWordCursor::operator == (const ZLTextWordCursor &cursor) const {
	return
		(myParagraphCursor->index() == cursor.myParagraphCursor->index()) &&
		(myWordNumber == cursor.myWordNumber) &&
		(myCharNumber == cursor.myCharNumber);
}
inline bool ZLTextWordCursor::operator != (const ZLTextWordCursor &cursor) const {
	return !operator == (cursor);
}
inline const ZLTextElement &ZLTextWordCursor::element() const { return (*myParagraphCursor)[myWordNumber]; }
inline bool ZLTextWordCursor::isStartOfParagraph() const {
	return (myWordNumber == 0) && (myCharNumber == 0);
}
inline bool ZLTextWordCursor::isEndOfParagraph() const {
	return myWordNumber == myParagraphCursor->paragraphLength();
}
inline unsigned int ZLTextWordCursor::wordNumber() const { return myWordNumber; }
inline unsigned int ZLTextWordCursor::charNumber() const { return myCharNumber; }
inline ZLTextParagraphCursorPtr ZLTextWordCursor::paragraphCursorPtr() const { return myParagraphCursor; }
inline const ZLTextParagraphCursor &ZLTextWordCursor::paragraphCursor() const { return *myParagraphCursor; }

inline void ZLTextWordCursor::nextWord() { ++myWordNumber; myCharNumber = 0; }
inline void ZLTextWordCursor::previousWord() { --myWordNumber; myCharNumber = 0; }

inline ZLTextPlainParagraphCursor::ZLTextPlainParagraphCursor(const ZLTextModel &model, size_t index) : ZLTextParagraphCursor(model, index) {}
inline ZLTextPlainParagraphCursor::~ZLTextPlainParagraphCursor() {}

inline ZLTextTreeParagraphCursor::ZLTextTreeParagraphCursor(const ZLTextTreeModel &model, size_t index) : ZLTextParagraphCursor(model, index) {}
inline ZLTextTreeParagraphCursor::~ZLTextTreeParagraphCursor() {}

#endif /* __ZLTEXTPARAGRAPHCURSOR_H__ */
