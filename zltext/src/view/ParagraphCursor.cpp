/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <algorithm>

#include <ZLTextParagraph.h>
#include <ZLTextHyphenator.h>

#include "ZLTextParagraphCursor.h"
#include "ZLTextWord.h"

TextElementPool TextElementPool::Pool;

std::map<const ZLTextParagraph*, weak_ptr<ParagraphCursor> > ParagraphCursorCache::ourCache;
ParagraphCursorPtr ParagraphCursorCache::ourLastAdded;

TextElementVector::~TextElementVector() {
	for (TextElementVector::const_iterator it = begin(); it != end(); ++it) {
		switch ((*it)->kind()) {
			case TextElement::WORD_ELEMENT:
				TextElementPool::Pool.storeWord((Word*)*it);
				break;
			case TextElement::CONTROL_ELEMENT:
				TextElementPool::Pool.storeControlElement((ControlElement*)*it);
				break;
			case TextElement::IMAGE_ELEMENT:
			case TextElement::FORCED_CONTROL_ELEMENT:
			case TextElement::FIXED_HSPACE_ELEMENT:
				delete *it;
				break;
			case TextElement::INDENT_ELEMENT:
			case TextElement::HSPACE_ELEMENT:
			case TextElement::BEFORE_PARAGRAPH_ELEMENT:
			case TextElement::AFTER_PARAGRAPH_ELEMENT:
			case TextElement::EMPTY_LINE_ELEMENT:
				break;
		}
	}
}

TextElementPool::TextElementPool() {
	HSpaceElement = new SpecialTextElement(TextElement::HSPACE_ELEMENT);
	BeforeParagraphElement = new SpecialTextElement(TextElement::BEFORE_PARAGRAPH_ELEMENT);
	AfterParagraphElement = new SpecialTextElement(TextElement::AFTER_PARAGRAPH_ELEMENT);
	EmptyLineElement = new SpecialTextElement(TextElement::EMPTY_LINE_ELEMENT);
}

TextElementPool::~TextElementPool() {
	delete HSpaceElement;
	delete BeforeParagraphElement;
	delete AfterParagraphElement;
	delete EmptyLineElement;
}

ParagraphCursorPtr ParagraphCursor::cursor(const ZLTextModel &model, size_t index) {
	ParagraphCursorPtr result = ParagraphCursorCache::get(model[index]);
	if (result.isNull()) {
		if (model.kind() == ZLTextModel::TREE_MODEL) {
			result = new TreeParagraphCursor((const ZLTextTreeModel&)model, index);
		} else {
			result = new PlainTextParagraphCursor((const ZLTextPlainModel&)model, index);
		}
		ParagraphCursorCache::put(model[index], result);
	}
	return result;
}

ParagraphCursor::ParagraphCursor(const ZLTextModel &model, size_t index) : myModel(model) {
	myIndex = std::min(index, myModel.paragraphsNumber() - 1);
	fill();
}

ParagraphCursor::~ParagraphCursor() {
}

ParagraphCursorPtr PlainTextParagraphCursor::previous() const {
	return isFirst() ? 0 : cursor(myModel, myIndex - 1);
}

ParagraphCursorPtr TreeParagraphCursor::previous() const {
	if (isFirst()) {
		return 0;
	}
	const ZLTextTreeParagraph *oldTreeParagraph = (const ZLTextTreeParagraph*)myModel[myIndex];
	const ZLTextTreeParagraph *parent = oldTreeParagraph->parent();
	size_t index = myIndex - 1;
	const ZLTextTreeParagraph *newTreeParagraph = (ZLTextTreeParagraph*)myModel[index];
	if (newTreeParagraph != parent) {
		const ZLTextTreeParagraph *lastNotOpen = newTreeParagraph;
		for (const ZLTextTreeParagraph *p = newTreeParagraph->parent(); p != parent; p = p->parent()) {
			if (!p->isOpen()) {
				lastNotOpen = p;
			}
		}
		while (myModel[index] != lastNotOpen) {
			--index;
		}
	}
	return cursor(myModel, index);
}

ParagraphCursorPtr PlainTextParagraphCursor::next() const {
	return isLast() ? 0 : cursor(myModel, myIndex + 1);
}

ParagraphCursorPtr TreeParagraphCursor::next() const {
	if (myIndex + 1 == myModel.paragraphsNumber()) {
		return 0;
	}
	const ZLTextTreeParagraph *current = (const ZLTextTreeParagraph*)myModel[myIndex];
	if (!current->children().empty() && current->isOpen()) {
		return cursor(myModel, myIndex + 1);
	}

	const ZLTextTreeParagraph *parent = current->parent();
	while ((parent != 0) && (current == parent->children().back())) {
		current = parent;
		parent = current->parent();
	}
	if (parent != 0) {
		size_t index = myIndex + 1;
		while (((const ZLTextTreeParagraph*)myModel[index])->parent() != parent) {
			++index;
		}
		return cursor(myModel, index);
	}
	return 0;
}

bool ParagraphCursor::isFirst() const {
	return
		(myIndex == 0) ||
		(myModel[myIndex]->kind() == ZLTextParagraph::END_OF_TEXT_PARAGRAPH) ||
		(myModel[myIndex - 1]->kind() == ZLTextParagraph::END_OF_TEXT_PARAGRAPH);
}

bool PlainTextParagraphCursor::isLast() const {
	return
		(myIndex + 1 == myModel.paragraphsNumber()) ||
		(myModel[myIndex + 1]->kind() == ZLTextParagraph::END_OF_TEXT_PARAGRAPH);
}

bool TreeParagraphCursor::isLast() const {
	if ((myIndex + 1 == myModel.paragraphsNumber()) ||
			(myModel[myIndex + 1]->kind() == ZLTextParagraph::END_OF_TEXT_PARAGRAPH)) {
		return true;
	}
	const ZLTextTreeParagraph *current = (const ZLTextTreeParagraph*)myModel[myIndex];
	if (current->isOpen() && !current->children().empty()) {
		return false;
	}
	const ZLTextTreeParagraph *parent = current->parent();
	while (parent != 0) {
		if (current != parent->children().back()) {
			return false;
		}
		current = parent;
		parent = current->parent();
	}
	return true;
}

bool ParagraphCursor::isEndOfSection() const {
	return myModel[myIndex]->kind() == ZLTextParagraph::END_OF_SECTION_PARAGRAPH;
}

ZLTextMark WordCursor::position() const {
	if (myParagraphCursor.isNull()) {
		return ZLTextMark();
	}
	const ParagraphCursor &paragraph = *myParagraphCursor;
	size_t paragraphLength = paragraph.paragraphLength();
	unsigned int wordNumber = myWordNumber;
	while ((wordNumber != paragraphLength) && (paragraph[wordNumber].kind() != TextElement::WORD_ELEMENT)) {
		++wordNumber;
	}
	if (wordNumber != paragraphLength) {
		return ZLTextMark(paragraph.index(), ((Word&)paragraph[wordNumber]).ParagraphOffset, 0);
	}
	return ZLTextMark(paragraph.index() + 1, 0, 0);
}

void ParagraphCursor::processControlParagraph(const ZLTextParagraph &paragraph) {
	for (ZLTextParagraph::Iterator it = paragraph; !it.isEnd(); it.next()) {
		myElements.push_back(TextElementPool::Pool.getControlElement(it.entry()));
	}
}

void ParagraphCursor::fill() {
	const ZLTextParagraph &paragraph = *myModel[myIndex];
	switch (paragraph.kind()) {
		case ZLTextParagraph::TEXT_PARAGRAPH:
		case ZLTextParagraph::TREE_PARAGRAPH:
		{
			const std::string &breakingAlgorithm = Hyphenator::instance().breakingAlgorithm();
			if (breakingAlgorithm == "chinese") {
				ChineseParagraphProcessor(paragraph, myModel.marks(), index(), myElements).fill();
			} else if (breakingAlgorithm == "anycharacter") {
				AnyPlaceParagraphProcessor(paragraph, myModel.marks(), index(), myElements).fill();
			} else {
				StandardParagraphProcessor(paragraph, myModel.marks(), index(), myElements).fill();
			}
			break;
		}
		case ZLTextParagraph::EMPTY_LINE_PARAGRAPH:
			processControlParagraph(paragraph);
			myElements.push_back(TextElementPool::Pool.EmptyLineElement);
			break;
		case ZLTextParagraph::BEFORE_SKIP_PARAGRAPH:
			processControlParagraph(paragraph);
			myElements.push_back(TextElementPool::Pool.BeforeParagraphElement);
			break;
		case ZLTextParagraph::AFTER_SKIP_PARAGRAPH:
			processControlParagraph(paragraph);
			myElements.push_back(TextElementPool::Pool.AfterParagraphElement);
			break;
		case ZLTextParagraph::END_OF_SECTION_PARAGRAPH:
		case ZLTextParagraph::END_OF_TEXT_PARAGRAPH:
			break;
	}
}

void ParagraphCursor::clear() {
	myElements.clear();
}

void ParagraphCursorCache::put(const ZLTextParagraph *paragraph, ParagraphCursorPtr cursor) {
	ourCache[paragraph] = cursor;
	ourLastAdded = cursor;
}

ParagraphCursorPtr ParagraphCursorCache::get(const ZLTextParagraph *paragraph) {
	return ourCache[paragraph];
}

void ParagraphCursorCache::clear() {
	ourLastAdded.reset();
	ourCache.clear();
}

void ParagraphCursorCache::cleanup() {
	std::map<const ZLTextParagraph*, weak_ptr<ParagraphCursor> > cleanedCache;
	for (std::map<const ZLTextParagraph*, weak_ptr<ParagraphCursor> >::iterator it = ourCache.begin(); it != ourCache.end(); ++it) {
		if (!it->second.isNull()) {
			cleanedCache.insert(*it);
		}
	}
	ourCache.swap(cleanedCache);
}

void WordCursor::rebuild() {
	if (!isNull()) {
		myParagraphCursor->clear();
		myParagraphCursor->fill();
	}
}

void WordCursor::setCharNumber(int charNumber) {
	charNumber = std::max(charNumber, 0);
	myCharNumber = 0;
	if (charNumber > 0) {
		const TextElement &element = (*myParagraphCursor)[myWordNumber];
		if (element.kind() == TextElement::WORD_ELEMENT) {
			if (charNumber <= (int)((const Word&)element).Length) {
				myCharNumber = charNumber;
			}
		}
	}
}

void WordCursor::moveTo(int wordNumber, int charNumber) {
	if (!isNull()) {
		if ((wordNumber == 0) && (charNumber == 0)) {
			myWordNumber = 0;
			myCharNumber = 0;
		} else {
			wordNumber = std::max(0, wordNumber);
			size_t size = myParagraphCursor->paragraphLength();
			if ((size_t)wordNumber > size) {
				myWordNumber = size;
				myCharNumber = 0;
			} else {
				myWordNumber = wordNumber;
				setCharNumber(charNumber);
			}
		}
	}
}

const WordCursor &WordCursor::operator = (ParagraphCursorPtr paragraphCursor) {
	myWordNumber = 0;
	myCharNumber = 0;
	myParagraphCursor = paragraphCursor;
	moveToParagraphStart();
	return *this;
}

void WordCursor::moveToParagraph(int paragraphNumber) {
	if (!isNull() && (paragraphNumber != (int)myParagraphCursor->index())) {
		myParagraphCursor = ParagraphCursor::cursor(myParagraphCursor->myModel, paragraphNumber);
		moveToParagraphStart();
	}
}

bool WordCursor::nextParagraph() {
	if (!isNull()) {
		if (!myParagraphCursor->isLast()) {
			myParagraphCursor = myParagraphCursor->next();
			moveToParagraphStart();
			return true;
		}
	}
	return false;
}

bool WordCursor::previousParagraph() {
	if (!isNull()) {
		if (!myParagraphCursor->isFirst()) {
			myParagraphCursor = myParagraphCursor->previous();
			moveToParagraphStart();
			return true;
		}
	}
	return false;
}

void WordCursor::moveToParagraphStart() {
	if (!isNull()) {
		myWordNumber = 0;
		myCharNumber = 0;
	}
}

void WordCursor::moveToParagraphEnd() {
	if (!isNull()) {
		myWordNumber = myParagraphCursor->paragraphLength();
		myCharNumber = 0;
	}
}

bool WordCursor::operator < (const WordCursor &cursor) const {
	int pn0 = myParagraphCursor->index();
	int pn1 = cursor.myParagraphCursor->index();
	if (pn0 < pn1) return true;
	if (pn1 < pn0) return false;
	if (myWordNumber < cursor.myWordNumber) return true;
	if (myWordNumber > cursor.myWordNumber) return false;
	return myCharNumber < cursor.myCharNumber;
}
