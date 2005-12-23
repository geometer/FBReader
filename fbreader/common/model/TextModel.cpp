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

TextModel::TextModel() : myLastEntryStart(0) {
}

TextModel::~TextModel() {
	for (std::vector<Paragraph*>::const_iterator it = myParagraphs.begin(); it != myParagraphs.end(); ++it) {
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
	for (std::vector<Paragraph*>::const_iterator it = start; it < end; ++it) {
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

void TextModel::selectParagraph(size_t index) const {
	if (index < paragraphsNumber()) {
		myMarks.clear();
		myMarks.push_back(TextMark(index, 0, (*this)[index]->textLength()));
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
		--it;
	}
	if (*it >= position) {
		if (it == marks().begin()) {
			return TextMark();
		}
		--it;
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
	myLastEntryStart = 0;
}

void TextModel::removeParagraphInternal(int index) {
	if ((index >= 0) && (index < (int)myParagraphs.size())) {
		myParagraphs.erase(myParagraphs.begin() + index);
	}
}

void TreeModel::removeParagraph(int index) {
	TreeParagraph *p = (TreeParagraph*)(*this)[index];
	p->removeFromParent();
	removeParagraphInternal(index);
	delete p;
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
	for (std::vector<TextMark>::const_iterator it = marks().begin(); it != marks().end(); ++it) {
		((TreeParagraph*)(*this)[it->ParagraphNumber])->openTree();
	}
}

void TreeModel::selectParagraph(size_t index) const {
	if (index < paragraphsNumber()) {
		TextModel::selectParagraph(index);
		((TreeParagraph*)(*this)[index])->openTree();
	}
}

void PlainTextModel::createParagraph(Paragraph::Kind kind) {
	Paragraph *paragraph = (kind == Paragraph::TEXT_PARAGRAPH) ? new Paragraph() : new SpecialParagraph(kind);
	addParagraphInternal(paragraph);
}

void PlainTextModel::createParagraphWithReference(long reference) {
	ParagraphWithReference *paragraph = new ParagraphWithReference(reference);
	addParagraphInternal(paragraph);
}

void TextModel::addText(const std::string &text) {
	size_t len = text.length();
	if ((myLastEntryStart != 0) && (*myLastEntryStart == ParagraphEntry::TEXT_ENTRY)) {
		size_t oldLen = 0;
		memcpy(&oldLen, myLastEntryStart + 1, sizeof(size_t));
		size_t newLen = oldLen + len;
		myLastEntryStart = myAllocator.reallocateLast(myLastEntryStart, newLen + sizeof(size_t) + 1);
		memcpy(myLastEntryStart + 1, &newLen, sizeof(size_t));
		memcpy(myLastEntryStart + sizeof(size_t) + 1 + oldLen, text.data(), len);
	} else {
		myLastEntryStart = myAllocator.allocate(len + sizeof(size_t) + 1);
		*myLastEntryStart = ParagraphEntry::TEXT_ENTRY;
		memcpy(myLastEntryStart + 1, &len, sizeof(size_t));
		memcpy(myLastEntryStart + sizeof(size_t) + 1, text.data(), len);
		myParagraphs.back()->addEntry(myLastEntryStart);
	}
}

void TextModel::addText(const std::vector<std::string> &text) {
	size_t len = 0;
	for (std::vector<std::string>::const_iterator it = text.begin(); it != text.end(); ++it) {
		len += it->length();
	}
	if ((myLastEntryStart != 0) && (*myLastEntryStart == ParagraphEntry::TEXT_ENTRY)) {
		size_t oldLen = 0;
		memcpy(&oldLen, myLastEntryStart + 1, sizeof(size_t));
		size_t newLen = oldLen + len;
		myLastEntryStart = myAllocator.reallocateLast(myLastEntryStart, newLen + sizeof(size_t) + 1);
		memcpy(myLastEntryStart + 1, &newLen, sizeof(size_t));
		size_t offset = sizeof(size_t) + 1 + oldLen;
		for (std::vector<std::string>::const_iterator it = text.begin(); it != text.end(); ++it) {
			memcpy(myLastEntryStart + offset, it->data(), it->length());
			offset += it->length();
		}
	} else {
		myLastEntryStart = myAllocator.allocate(len + sizeof(size_t) + 1);
		*myLastEntryStart = ParagraphEntry::TEXT_ENTRY;
		memcpy(myLastEntryStart + 1, &len, sizeof(size_t));
		size_t offset = sizeof(size_t) + 1;
		for (std::vector<std::string>::const_iterator it = text.begin(); it != text.end(); ++it) {
			memcpy(myLastEntryStart + offset, it->data(), it->length());
			offset += it->length();
		}
		myParagraphs.back()->addEntry(myLastEntryStart);
	}
}

void TextModel::addControl(TextKind textKind, bool isStart) {
	myLastEntryStart = myAllocator.allocate(2);
	*myLastEntryStart = ParagraphEntry::CONTROL_ENTRY;
	*(myLastEntryStart + 1) = (textKind << 1) + (isStart ? 1 : 0);
	myParagraphs.back()->addEntry(myLastEntryStart);
}

void TextModel::addControl(const ForcedControlEntry &entry) {
	myLastEntryStart = myAllocator.allocate(3 + 2 * sizeof(short));
	*myLastEntryStart = ParagraphEntry::FORCED_CONTROL_ENTRY;
	*(myLastEntryStart + 1) = entry.myMask;
	memcpy(myLastEntryStart + 2, &entry.myLeftIndent, sizeof(short));
	memcpy(myLastEntryStart + 2 + sizeof(short), &entry.myRightIndent, sizeof(short));
	*(myLastEntryStart + 2 + 2 * sizeof(short)) = entry.myAlignmentType;
	myParagraphs.back()->addEntry(myLastEntryStart);
}

void TextModel::addHyperlinkControl(TextKind textKind, const std::string &label) {
	myLastEntryStart = myAllocator.allocate(label.length() + 3);
	*myLastEntryStart = ParagraphEntry::HYPERLINK_CONTROL_ENTRY;
	*(myLastEntryStart + 1) = textKind;
	memcpy(myLastEntryStart + 2, label.data(), label.length());
	*(myLastEntryStart + label.length() + 2) = '\0';
	myParagraphs.back()->addEntry(myLastEntryStart);
}

void TextModel::addImage(const std::string &id, const ImageMap &imageMap) {
	myLastEntryStart = myAllocator.allocate(sizeof(const ImageMap*) + id.length() + 2);
	*myLastEntryStart = ParagraphEntry::IMAGE_ENTRY;
	const ImageMap *imageMapAddress = &imageMap;
	memcpy(myLastEntryStart + 1, &imageMapAddress, sizeof(const ImageMap*));
	memcpy(myLastEntryStart + 1 + sizeof(const ImageMap*), id.data(), id.length());
	*(myLastEntryStart + 1 + sizeof(const ImageMap*) + id.length()) = '\0';
	myParagraphs.back()->addEntry(myLastEntryStart);
}
