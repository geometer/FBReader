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

#include <algorithm>

#include "ParagraphCursor.h"
#include "Word.h"

#include "../model/Paragraph.h"

TextElementPool TextElementPool::Pool;

int ParagraphCursor::Cache::ourCacheCounter = 0;
std::map<Paragraph*,shared_ptr<TextElementVector> > ParagraphCursor::Cache::ourCache;

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
		return new TreeParagraphCursor((TreeModel&)model);
	}
	return new PlainTextParagraphCursor((PlainTextModel&)model);
}

ParagraphCursor::ParagraphCursor(const TextModel &model) : myModel(model) {
	myParagraphIterator = myModel.paragraphs().begin();
	fill();
}

ParagraphCursor::ParagraphCursor(const ParagraphCursor &cursor) : myModel(cursor.myModel) {
	if (!myElements.isNull() && (ParagraphCursor::Cache::ourCacheCounter > 0)) {
		ParagraphCursor::Cache::ourCache[*myParagraphIterator] = myElements;
	}
	myParagraphIterator = cursor.myParagraphIterator;
	myElements = cursor.myElements;
	myNextElement = cursor.myNextElement;
}

ParagraphCursor::~ParagraphCursor() {
}

bool PlainTextParagraphCursor::previous() {
	if (myParagraphIterator == myModel.paragraphs().begin()) {
		return false;
	}
	clear();
	myParagraphIterator--;
	fill();
	return true;
}

bool TreeParagraphCursor::previous() {
	if (myParagraphIterator == myModel.paragraphs().begin()) {
		return false;
	}
	clear();
	TreeParagraph *oldTreeParagraph = (TreeParagraph*)*myParagraphIterator;
	TreeParagraph *parent = oldTreeParagraph->parent();
	myParagraphIterator--;
	TreeParagraph *newTreeParagraph = (TreeParagraph*)*myParagraphIterator;
	if (newTreeParagraph != parent) {
		TreeParagraph *lastNotOpen = newTreeParagraph;
		for (TreeParagraph *p = newTreeParagraph->parent(); p != parent; p = p->parent()) {
			if (!p->isOpen()) {
				lastNotOpen = p;
			}
		}
		while (*myParagraphIterator != lastNotOpen) {
			myParagraphIterator--;
		}
	}
	fill();
	return true;
}

bool PlainTextParagraphCursor::next() {
	if (myParagraphIterator + 1 == myModel.paragraphs().end()) {
		return false;
	}
	clear();
	myParagraphIterator++;
	fill();
	return true;
}

bool TreeParagraphCursor::next() {
	if (myParagraphIterator + 1 == myModel.paragraphs().end()) {
		return false;
	}
	TreeParagraph *current = (TreeParagraph*)*myParagraphIterator;
	if (!current->children().empty() && current->isOpen()) {
		clear();
		myParagraphIterator++;
		fill();
		return true;
	}

	TreeParagraph *parent = current->parent();
	while ((parent != 0) && (current == parent->children().back())) {
		current = parent;
		parent = current->parent();
	}
	if (parent == 0) {
		return false;
	}
	clear();
	do {
		myParagraphIterator++;
	} while (((TreeParagraph*)*myParagraphIterator)->parent() != parent);
	fill();
	return true;
}

bool ParagraphCursor::isStartOfText() const {
	return (myParagraphIterator == myModel.paragraphs().begin()) && isStartOfParagraph();
}

bool PlainTextParagraphCursor::isEndOfText() const {
	return isEndOfParagraph() && (myParagraphIterator + 1 == myModel.paragraphs().end());
}

bool TreeParagraphCursor::isEndOfText() const {
	if (!isEndOfParagraph()) {
		return false;
	}
	if (myParagraphIterator + 1 == myModel.paragraphs().end()) {
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

bool ParagraphCursor::isStartOfParagraph() const {
	return (myNextElement.myWordIterator == myElements->begin()) && (myNextElement.myCharNumber == 0);
}

bool ParagraphCursor::isEndOfParagraph() const {
	return myNextElement.myWordIterator == myElements->end();
}

bool ParagraphCursor::isEndOfSection() const {
	return (*myParagraphIterator)->kind() == Paragraph::EOS_PARAGRAPH;
}

TextMark ParagraphCursor::position() const {
	WordCursor cursor = myNextElement;
	while ((cursor.myWordIterator != myElements->end()) &&
				 (cursor.element().kind() != TextElement::WORD_ELEMENT)) {
		cursor.nextWord();
	}
	if (cursor.myWordIterator != myElements->end()) {
		return TextMark(paragraphNumber(), ((Word&)cursor.element()).ParagraphOffset, 0);
	}
	return TextMark(paragraphNumber() + 1, 0, 0);
}

void ParagraphCursor::processControlParagraph(const Paragraph &paragraph) {
	const std::vector<ParagraphEntry*> &entries = paragraph.entries();
	for (std::vector<ParagraphEntry*>::const_iterator it = entries.begin(); it != entries.end(); it++) {
		ControlEntry &control = *(ControlEntry*)*it;
		myElements->push_back(TextElementPool::Pool.getControlElement(control));
	}
}

void ParagraphCursor::fill() {
	if (ParagraphCursor::Cache::ourCacheCounter > 0) {
		myElements = ParagraphCursor::Cache::ourCache[*myParagraphIterator];
	} else {
		myElements = 0;
	}
	if (myElements.isNull()) {
		myElements = new TextElementVector();
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
				myElements->push_back(TextElementPool::Pool.EmptyLineElement);
				break;
			}
			case Paragraph::BEFORE_SKIP_PARAGRAPH:
			{
				processControlParagraph(*(Paragraph*)*myParagraphIterator);
				myElements->push_back(TextElementPool::Pool.BeforeParagraphElement);
				break;
			}
			case Paragraph::AFTER_SKIP_PARAGRAPH:
			{
				processControlParagraph(*(Paragraph*)*myParagraphIterator);
				myElements->push_back(TextElementPool::Pool.AfterParagraphElement);
				break;
			}
			case Paragraph::EOS_PARAGRAPH:
				break;
		}
	}

	myNextElement = myElements->begin();
}

void ParagraphCursor::clear() {
	if (!myElements.isNull() && (ParagraphCursor::Cache::ourCacheCounter > 0)) {
		ParagraphCursor::Cache::ourCache[*myParagraphIterator] = myElements;
	}
	myElements = 0;
}

void ParagraphCursor::rebuild() {
	int w = wordNumber(myNextElement);
	int c = myNextElement.myCharNumber;
	clear();
	fill();
	myNextElement = myElements->begin() + w;
	myNextElement.myCharNumber = c;
}

void ParagraphCursor::moveTo(int paragraphNumber) {
	std::vector<Paragraph*>::const_iterator it =
		myModel.paragraphs().begin() + std::min(paragraphNumber, (int)myModel.paragraphs().size() - 1);
	if (myParagraphIterator != it) {
		clear();
		myParagraphIterator = it;
		fill();
	}
}

WordCursor ParagraphCursor::wordCursor(int wordNumber, int charNumber) const {
	wordNumber = std::max(0, wordNumber);
	if (wordNumber > (int)myElements->size()) {
		wordNumber = myElements->size();
		charNumber = 0;
	}
	WordCursor cursor(myElements->begin() + wordNumber);
	cursor.setCharNumber(charNumber);
	return cursor;
}

void WordCursor::setCharNumber(int charNumber) {
	charNumber = std::max(charNumber, 0);
	myCharNumber = 0;
	if (charNumber > 0) {
		const TextElement &element = **myWordIterator;
		if (element.kind() == TextElement::WORD_ELEMENT) {
			myCharNumber = std::min(charNumber, (int)((const Word&)element).Length - 1);
		}
	}
}
