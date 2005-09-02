/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "ParagraphCursor.h"
#include "Word.h"

#include "../model/Paragraph.h"

TextElementPool TextElementPool::Pool;

std::map<Paragraph*, weak_ptr<ParagraphCursor> > ParagraphCursorCache::ourCache;

TextElementVector::~TextElementVector() {
	for (TextElementVector::const_iterator it = begin(); it != end(); it++) {
		switch ((*it)->kind()) {
			case TextElement::WORD_ELEMENT:
				TextElementPool::Pool.storeWord((Word*)*it);
				break;
			case TextElement::CONTROL_ELEMENT:
				TextElementPool::Pool.storeControlElement((ControlElement*)*it);
				break;
			case TextElement::IMAGE_ELEMENT:
			case TextElement::TREE_ELEMENT:
			case TextElement::FORCED_CONTROL_ELEMENT:
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
	IndentElement = new SpecialTextElement(TextElement::INDENT_ELEMENT);
	EmptyLineElement = new SpecialTextElement(TextElement::EMPTY_LINE_ELEMENT);
}

TextElementPool::~TextElementPool() {
	delete HSpaceElement;
	delete BeforeParagraphElement;
	delete AfterParagraphElement;
	delete IndentElement;
	delete EmptyLineElement;
}

ParagraphCursor *ParagraphCursor::createCursor(const TextModel &model) {
	if (model.kind() == TextModel::TREE_MODEL) {
		return new TreeParagraphCursor((const TreeModel&)model, model.paragraphs().begin());
	}
	return new PlainTextParagraphCursor((const PlainTextModel&)model, model.paragraphs().begin());
}

ParagraphCursor::ParagraphCursor(const TextModel &model, const ParagraphIterator &iterator) : myModel(model), myParagraphIterator(iterator) {
	fill();
}

ParagraphCursor::~ParagraphCursor() {
}

shared_ptr<ParagraphCursor> ParagraphCursor::cursor(const ParagraphIterator &iterator) const {
	shared_ptr<ParagraphCursor> result = ParagraphCursorCache::get(*iterator);
	if (result.isNull()) {
		result = createCursor(iterator);
		ParagraphCursorCache::put(*iterator, result);
	}
	return result;
}

shared_ptr<ParagraphCursor> PlainTextParagraphCursor::previous() const {
	return isFirst() ? 0 : cursor(myParagraphIterator - 1);
}

shared_ptr<ParagraphCursor> TreeParagraphCursor::previous() const {
	if (isFirst()) {
		return 0;
	}
	ParagraphIterator it = myParagraphIterator;
	TreeParagraph *oldTreeParagraph = (TreeParagraph*)*it;
	TreeParagraph *parent = oldTreeParagraph->parent();
	it--;
	TreeParagraph *newTreeParagraph = (TreeParagraph*)*it;
	if (newTreeParagraph != parent) {
		TreeParagraph *lastNotOpen = newTreeParagraph;
		for (TreeParagraph *p = newTreeParagraph->parent(); p != parent; p = p->parent()) {
			if (!p->isOpen()) {
				lastNotOpen = p;
			}
		}
		while (*it != lastNotOpen) {
			it--;
		}
	}
	return cursor(it);
}

shared_ptr<ParagraphCursor> PlainTextParagraphCursor::next() const {
	return isLast() ? 0 : cursor(myParagraphIterator + 1);
}

shared_ptr<ParagraphCursor> TreeParagraphCursor::next() const {
	if (myParagraphIterator + 1 == myModel.paragraphs().end()) {
		return 0;
	}
	TreeParagraph *current = (TreeParagraph*)*myParagraphIterator;
	if (!current->children().empty() && current->isOpen()) {
		return cursor(myParagraphIterator + 1);
	}

	TreeParagraph *parent = current->parent();
	while ((parent != 0) && (current == parent->children().back())) {
		current = parent;
		parent = current->parent();
	}
	if (parent != 0) {
		ParagraphIterator it = myParagraphIterator;
		do {
			it++;
		} while (((TreeParagraph*)*it)->parent() != parent);
		return cursor(it);
	}
	return 0;
}

bool ParagraphCursor::isFirst() const {
	return
		(myParagraphIterator == myModel.paragraphs().begin()) ||
		((*(myParagraphIterator - 1))->kind() == Paragraph::END_OF_TEXT_PARAGRAPH);
}

bool PlainTextParagraphCursor::isLast() const {
	return
		(myParagraphIterator + 1 == myModel.paragraphs().end()) ||
		((*(myParagraphIterator + 1))->kind() == Paragraph::END_OF_TEXT_PARAGRAPH);
}

bool TreeParagraphCursor::isLast() const {
	if ((myParagraphIterator + 1 == myModel.paragraphs().end()) ||
			((*(myParagraphIterator + 1))->kind() == Paragraph::END_OF_TEXT_PARAGRAPH)) {
		return true;
	}
	TreeParagraph *current = (TreeParagraph*)*myParagraphIterator;
	if (current->isOpen() && !current->children().empty()) {
		return false;
	}
	TreeParagraph *parent = current->parent();
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
	return (*myParagraphIterator)->kind() == Paragraph::END_OF_SECTION_PARAGRAPH;
}

TextMark WordCursor::position() const {
	const ParagraphCursor &paragraph = *myParagraphCursor;
	unsigned int paragraphLength = paragraph.paragraphLength();
	unsigned int wordNumber = myWordNumber;
	while ((wordNumber != paragraphLength) && (paragraph[wordNumber].kind() != TextElement::WORD_ELEMENT)) {
		wordNumber++;
	}
	if (wordNumber != paragraphLength) {
		return TextMark(paragraph.paragraphNumber(), ((Word&)paragraph[wordNumber]).ParagraphOffset, 0);
	}
	return TextMark(paragraph.paragraphNumber() + 1, 0, 0);
}

void ParagraphCursor::processControlParagraph(const Paragraph &paragraph) {
	const std::vector<ParagraphEntry*> &entries = paragraph.entries();
	for (std::vector<ParagraphEntry*>::const_iterator it = entries.begin(); it != entries.end(); it++) {
		ControlEntry &control = *(ControlEntry*)*it;
		myElements.push_back(TextElementPool::Pool.getControlElement(control));
	}
}

void ParagraphCursor::fill() {
	switch ((*myParagraphIterator)->kind()) {
		case Paragraph::TEXT_PARAGRAPH:
		case Paragraph::TREE_PARAGRAPH:
		{
			ParagraphProcessor processor(*(Paragraph*)*myParagraphIterator, myModel.marks(), paragraphNumber(), myElements);
			processor.fill();
			break;
		}
		case Paragraph::EMPTY_LINE_PARAGRAPH:
		{
			processControlParagraph(*(Paragraph*)*myParagraphIterator);
			myElements.push_back(TextElementPool::Pool.EmptyLineElement);
			break;
		}
		case Paragraph::BEFORE_SKIP_PARAGRAPH:
		{
			processControlParagraph(*(Paragraph*)*myParagraphIterator);
			myElements.push_back(TextElementPool::Pool.BeforeParagraphElement);
			break;
		}
		case Paragraph::AFTER_SKIP_PARAGRAPH:
		{
			processControlParagraph(*(Paragraph*)*myParagraphIterator);
			myElements.push_back(TextElementPool::Pool.AfterParagraphElement);
			break;
		}
		case Paragraph::END_OF_SECTION_PARAGRAPH:
		case Paragraph::END_OF_TEXT_PARAGRAPH:
			break;
	}
}

void ParagraphCursor::clear() {
	myElements.clear();
}

void ParagraphCursorCache::put(Paragraph *paragraph, shared_ptr<ParagraphCursor> cursor) {
	ourCache[paragraph] = cursor;
}

shared_ptr<ParagraphCursor> ParagraphCursorCache::get(Paragraph *paragraph) {
	return ourCache[paragraph];
}

void ParagraphCursorCache::clear() {
	ourCache.clear();
}

void ParagraphCursorCache::cleanup() {
	std::map<Paragraph*, weak_ptr<ParagraphCursor> > cleanedCache;
	for (std::map<Paragraph*, weak_ptr<ParagraphCursor> >::iterator it = ourCache.begin(); it != ourCache.end(); it++) {
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

shared_ptr<ParagraphCursor> ParagraphCursor::cursor(int paragraphNumber) const {
	return cursor(
		myModel.paragraphs().begin() + std::min((size_t)paragraphNumber, myModel.paragraphs().size() - 1)
	);
}

void WordCursor::setCharNumber(int charNumber) {
	charNumber = std::max(charNumber, 0);
	myCharNumber = 0;
	if (charNumber > 0) {
		const TextElement &element = (*myParagraphCursor)[myWordNumber];
		if (element.kind() == TextElement::WORD_ELEMENT) {
			myCharNumber = std::min(charNumber, (int)((const Word&)element).Length - 1);
		}
	}
}

void WordCursor::moveTo(int wordNumber, int charNumber) {
	if (!isNull()) {
		wordNumber = std::max(0, wordNumber);
		int size = myParagraphCursor->paragraphLength();
		if (wordNumber > size) {
			myWordNumber = size;
			myCharNumber = 0;
		} else {
			myWordNumber = wordNumber;
			setCharNumber(charNumber);
		}
	}
}

const WordCursor &WordCursor::operator = (ParagraphCursor *paragraphCursor) {
	myParagraphCursor = paragraphCursor;
	moveToParagraphStart();
	return *this;
}

void WordCursor::moveToParagraph(int paragraphNumber) {
	if (!isNull() && (paragraphNumber != (int)myParagraphCursor->paragraphNumber())) {
		myParagraphCursor = myParagraphCursor->cursor(paragraphNumber);
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
	int pn0 = myParagraphCursor->paragraphNumber();
	int pn1 = cursor.myParagraphCursor->paragraphNumber();
	if (pn0 < pn1) return true;
	if (pn1 < pn0) return false;
	if (myWordNumber < cursor.myWordNumber) return true;
	if (myWordNumber > cursor.myWordNumber) return false;
	return myCharNumber < cursor.myCharNumber;
}
