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
	myParagraphIterator = cursor.myParagraphIterator;
	fill();
	myNextElement.myWordIterator += cursor.wordNumber();
	myNextElement.myCharNumber = cursor.myNextElement.myCharNumber;
}

ParagraphCursor::~ParagraphCursor() {
	clear();
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
	return (myNextElement.myWordIterator == myElements.begin()) && (myNextElement.myCharNumber == 0);
}

bool ParagraphCursor::isEndOfParagraph() const {
	return myNextElement.myWordIterator == myElements.end();
}

bool ParagraphCursor::isEndOfSection() const {
	return (*myParagraphIterator)->kind() == Paragraph::EOS_PARAGRAPH;
}

TextMark ParagraphCursor::position() const {
	WordCursor cursor = wordCursor();
	while ((cursor.myWordIterator != myElements.end()) &&
				 (cursor.element().kind() != TextElement::WORD_ELEMENT)) {
		cursor.nextWord();
	}
	if (cursor.myWordIterator != myElements.end()) {
		return TextMark(paragraphNumber(), ((Word&)cursor.element()).startOffset(), 0);
	}
	return TextMark(paragraphNumber() + 1, 0, 0);
}

void ParagraphCursor::processControlParagraph(const Paragraph &paragraph) {
	const std::vector<ParagraphEntry*> entries = paragraph.entries();
	for (std::vector<ParagraphEntry*>::const_iterator it = entries.begin(); it != entries.end(); it++) {
		ControlEntry &control = *(ControlEntry*)*it;
		myElements.push_back(new ControlElement(control));
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
			myElements.push_back(new SpecialTextElement(TextElement::EMPTY_LINE_ELEMENT));
			break;
		}
		case Paragraph::BEFORE_SKIP_PARAGRAPH:
		{
			processControlParagraph(*(Paragraph*)*myParagraphIterator);
			myElements.push_back(new SpecialTextElement(TextElement::BEFORE_PARAGRAPH_ELEMENT));
			break;
		}
		case Paragraph::AFTER_SKIP_PARAGRAPH:
		{
			processControlParagraph(*(Paragraph*)*myParagraphIterator);
			myElements.push_back(new SpecialTextElement(TextElement::AFTER_PARAGRAPH_ELEMENT));
			break;
		}
		case Paragraph::EOS_PARAGRAPH:
			break;
	}

	myNextElement = myElements.begin();
}

void ParagraphCursor::clear() {
	for (std::vector<TextElement*>::const_iterator it = myElements.begin(); it != myElements.end(); it++) {
		delete *it;
	}
	myElements.clear();
}

void ParagraphCursor::moveTo(int paragraphNumber, int wordNumber, int charNumber) {
	if ((unsigned int)paragraphNumber >= myModel.paragraphs().size()) {
		return;
	}

	std::vector<Paragraph*>::const_iterator it = myModel.paragraphs().begin() + paragraphNumber;
	if (myParagraphIterator != it) {
		clear();
		myParagraphIterator = it;
		fill();
	}

	if (myElements.size() >= (unsigned int)wordNumber) {
		myNextElement = myElements.begin() + wordNumber;
		if ((myNextElement.myWordIterator != myElements.end()) &&
				(myNextElement.element().kind() == TextElement::WORD_ELEMENT)) {
			if (charNumber < ((const Word&)myNextElement.element()).length()) {
				myNextElement.myCharNumber = charNumber;
			}
		}
	}
}

void ParagraphCursor::moveToParagraphStart() {
	myNextElement = myElements.begin();
}

void ParagraphCursor::moveToParagraphEnd() {
	myNextElement = myElements.end();
}
