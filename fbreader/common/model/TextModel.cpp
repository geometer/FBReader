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

#include <abstract/ZLSearchUtil.h>

#include "TextModel.h"
#include "Paragraph.h"

TextModel::TextModel() {
}

TextModel::~TextModel() {
	for (std::vector<Paragraph*>::const_iterator it = myParagraphs.begin(); it != myParagraphs.end(); it++) {
		delete *it;
	}
}

void TextModel::search(const std::string &text, size_t startIndex, size_t endIndex, bool ignoreCase) const {
	ZLSearchPattern pattern(text, ignoreCase);
	myMarks.clear();

	std::vector<Paragraph*>::const_iterator start =
		(startIndex < myParagraphs.size()) ? myParagraphs.begin() + startIndex : myParagraphs.end();
	std::vector<Paragraph*>::const_iterator end =
		(endIndex < myParagraphs.size()) ? myParagraphs.begin() + endIndex : myParagraphs.end();
	for (std::vector<Paragraph*>::const_iterator it = start; it < end; it++) {
		int offset = 0;
		for (Paragraph::Iterator jt = **it; !jt.isEnd(); jt.next()) {
			if (jt.entryKind() == ParagraphEntry::TEXT_ENTRY) {
				const TextEntry& textEntry = (TextEntry&)*jt.entry();
				const char *str = textEntry.data();
				const size_t len = textEntry.dataLength();
				for (int pos = ZLSearchUtil::find(str, len, pattern); pos != -1; pos = ZLSearchUtil::find(str, len, pattern, pos + 1)) {
					myMarks.push_back(TextMark(it - myParagraphs.begin(), offset + pos, pattern.length()));
				}
				offset += len;
			}
		}
	}
}

void TextModel::selectParagraph(const unsigned int paragraphNumber) const {
	if (paragraphNumber < paragraphs().size()) {
		myMarks.clear();
		myMarks.push_back(TextMark(paragraphNumber, 0, paragraphs()[paragraphNumber]->textLength()));
	}
}

TextMark TextModel::firstMark() const {
	return marks().empty() ? TextMark() : marks().front();
}

TextMark TextModel::lastMark() const {
	return marks().empty() ? TextMark() : marks().back();
}

TextMark TextModel::nextMark(TextMark position) const {
	std::vector<TextMark>::const_iterator it = std::upper_bound(marks().begin(), marks().end(), position);
	return (it != marks().end()) ? *it : TextMark();
}

TextMark TextModel::previousMark(TextMark position) const {
	if (marks().empty()) {
		return TextMark();
	}
	std::vector<TextMark>::const_iterator it = std::lower_bound(marks().begin(), marks().end(), position);
	if (it == marks().end()) {
		it--;
	}
	if (*it >= position) {
		if (it == marks().begin()) {
			return TextMark();
		}
		it--;
	}
	return *it;
}

TreeModel::TreeModel() {
	myRoot = new TreeParagraph();
	myRoot->open(true);
}

TreeModel::~TreeModel() {
	delete myRoot;
}

void TextModel::addParagraphInternal(Paragraph *paragraph) {
	myParagraphs.push_back(paragraph);
}

TreeParagraph *TreeModel::createParagraph(TreeParagraph *parent) {
	if (parent == 0) {
		parent = myRoot;
	}
	TreeParagraph *tp = new TreeParagraph(parent);
	addParagraphInternal(tp);
	return tp;
}

void TreeModel::search(const std::string &text, size_t startIndex, size_t endIndex, bool ignoreCase) const {
	TextModel::search(text, startIndex, endIndex, ignoreCase);
	for (std::vector<TextMark>::const_iterator it = marks().begin(); it != marks().end(); it++) {
		((TreeParagraph*)paragraphs()[it->ParagraphNumber])->openTree();
	}
}

void TreeModel::selectParagraph(const unsigned int paragraphNumber) const {
	if (paragraphNumber < paragraphs().size()) {
		TextModel::selectParagraph(paragraphNumber);
		((TreeParagraph*)paragraphs()[paragraphNumber])->openTree();
	}
}

Paragraph *PlainTextModel::createParagraph(Paragraph::Kind kind) {
	Paragraph *paragraph = (kind == Paragraph::TEXT_PARAGRAPH) ? new Paragraph() : new SpecialParagraph(kind);
	addParagraphInternal(paragraph);
	return paragraph;
}

ParagraphWithReference *PlainTextModel::createParagraphWithReference(long reference) {
	ParagraphWithReference *paragraph = new ParagraphWithReference(reference);
	addParagraphInternal(paragraph);
	return paragraph;
}

void TextModel::addText(const std::string &text) {
	size_t len = text.length();
	char *address = (char*)myAllocator.allocate(len + sizeof(size_t) + 1);
	*address = ParagraphEntry::TEXT_ENTRY;
	memcpy(address + 1, &len, sizeof(size_t));
	memcpy(address + sizeof(size_t) + 1, text.data(), len);
	myParagraphs.back()->addEntry(address);
}

void TextModel::addText(const std::vector<std::string> &text) {
	size_t len = 0;
	for (std::vector<std::string>::const_iterator it = text.begin(); it != text.end(); it++) {
		len += it->length();
	}
	char *address = (char*)myAllocator.allocate(len + sizeof(size_t) + 1);
	*address = ParagraphEntry::TEXT_ENTRY;
	memcpy(address + 1, &len, sizeof(size_t));
	size_t offset = sizeof(size_t) + 1;
	for (std::vector<std::string>::const_iterator it = text.begin(); it != text.end(); it++) {
		memcpy(address + offset, it->data(), it->length());
		offset += it->length();
	}
	myParagraphs.back()->addEntry(address);
}

void TextModel::addControl(TextKind textKind, bool isStart) {
	char *address = (char*)myAllocator.allocate(2);
	*address = ParagraphEntry::CONTROL_ENTRY;
	*(address + 1) = (textKind << 1) + (isStart ? 1 : 0);
	myParagraphs.back()->addEntry(address);
}

void TextModel::addControl(const ForcedControlEntry &entry) {
	char *address = (char*)myAllocator.allocate(3 + 2 * sizeof(short));
	*address = ParagraphEntry::FORCED_CONTROL_ENTRY;
	*(address + 1) = entry.myMask;
	memcpy(address + 2, &entry.myLeftIndent, sizeof(short));
	memcpy(address + 2 + sizeof(short), &entry.myRightIndent, sizeof(short));
	*(address + 2 + 2 * sizeof(short)) = entry.myAlignmentType;
	myParagraphs.back()->addEntry(address);
}

void TextModel::addHyperlinkControl(TextKind textKind, const std::string &label) {
	char *address = (char*)myAllocator.allocate(label.length() + 3);
	*address = ParagraphEntry::HYPERLINK_CONTROL_ENTRY;
	*(address + 1) = textKind;
	memcpy(address + 2, label.data(), label.length());
	*(address + label.length() + 2) = '\0';
	myParagraphs.back()->addEntry(address);
}

void TextModel::addImage(const std::string &id, const ImageMap &imageMap) {
	char *address = (char*)myAllocator.allocate(sizeof(const ImageMap*) + id.length() + 2);
	*address = ParagraphEntry::IMAGE_ENTRY;
	const ImageMap *imageMapAddress = &imageMap;
	memcpy(address + 1, &imageMapAddress, sizeof(const ImageMap*));
	memcpy(address + 1 + sizeof(const ImageMap*), id.data(), id.length());
	*(address + 1 + sizeof(const ImageMap*) + id.length()) = '\0';
	myParagraphs.back()->addEntry(address);
}
