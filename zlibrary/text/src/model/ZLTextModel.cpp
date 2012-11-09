/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <cstring>
#include <algorithm>

#include <ZLibrary.h>
#include <ZLSearchUtil.h>
#include <ZLLanguageUtil.h>

#include "ZLTextModel.h"
#include "ZLTextParagraph.h"
#include "ZLTextStyleEntry.h"

ZLTextModel::ZLTextModel(const std::string &language, const std::size_t rowSize) : myLanguage(language.empty() ? ZLibrary::Language() : language), myAllocator(rowSize), myLastEntryStart(0) {
}

ZLTextModel::~ZLTextModel() {
	for (std::vector<ZLTextParagraph*>::const_iterator it = myParagraphs.begin(); it != myParagraphs.end(); ++it) {
		delete *it;
	}
}

const std::string &ZLTextModel::language() const {
	return myLanguage;
}

bool ZLTextModel::isRtl() const {
	return ZLLanguageUtil::isRTLLanguage(myLanguage);
}

void ZLTextModel::search(const std::string &text, std::size_t startIndex, std::size_t endIndex, bool ignoreCase) const {
	ZLSearchPattern pattern(text, ignoreCase);
	myMarks.clear();

	std::vector<ZLTextParagraph*>::const_iterator start =
		(startIndex < myParagraphs.size()) ? myParagraphs.begin() + startIndex : myParagraphs.end();
	std::vector<ZLTextParagraph*>::const_iterator end =
		(endIndex < myParagraphs.size()) ? myParagraphs.begin() + endIndex : myParagraphs.end();
	for (std::vector<ZLTextParagraph*>::const_iterator it = start; it < end; ++it) {
		int offset = 0;
		for (ZLTextParagraph::Iterator jt = **it; !jt.isEnd(); jt.next()) {
			if (jt.entryKind() == ZLTextParagraphEntry::TEXT_ENTRY) {
				const ZLTextEntry& textEntry = (ZLTextEntry&)*jt.entry();
				const char *str = textEntry.data();
				const std::size_t len = textEntry.dataLength();
				for (int pos = ZLSearchUtil::find(str, len, pattern); pos != -1; pos = ZLSearchUtil::find(str, len, pattern, pos + 1)) {
					myMarks.push_back(ZLTextMark(it - myParagraphs.begin(), offset + pos, pattern.length()));
				}
				offset += len;
			}
		}
	}
}

void ZLTextModel::selectParagraph(std::size_t index) const {
	if (index < paragraphsNumber()) {
		myMarks.push_back(ZLTextMark(index, 0, (*this)[index]->textDataLength()));
	}
}

ZLTextMark ZLTextModel::firstMark() const {
	return marks().empty() ? ZLTextMark() : marks().front();
}

ZLTextMark ZLTextModel::lastMark() const {
	return marks().empty() ? ZLTextMark() : marks().back();
}

ZLTextMark ZLTextModel::nextMark(ZLTextMark position) const {
	std::vector<ZLTextMark>::const_iterator it = std::upper_bound(marks().begin(), marks().end(), position);
	return (it != marks().end()) ? *it : ZLTextMark();
}

ZLTextMark ZLTextModel::previousMark(ZLTextMark position) const {
	if (marks().empty()) {
		return ZLTextMark();
	}
	std::vector<ZLTextMark>::const_iterator it = std::lower_bound(marks().begin(), marks().end(), position);
	if (it == marks().end()) {
		--it;
	}
	if (*it >= position) {
		if (it == marks().begin()) {
			return ZLTextMark();
		}
		--it;
	}
	return *it;
}

ZLTextTreeModel::ZLTextTreeModel(const std::string &language) : ZLTextModel(language, 8192) {
	myRoot = new ZLTextTreeParagraph();
	myRoot->open(true);
}

ZLTextTreeModel::~ZLTextTreeModel() {
	delete myRoot;
}

void ZLTextModel::addParagraphInternal(ZLTextParagraph *paragraph) {
	myParagraphs.push_back(paragraph);
	myLastEntryStart = 0;
}

ZLTextTreeParagraph *ZLTextTreeModel::createParagraph(ZLTextTreeParagraph *parent) {
	if (parent == 0) {
		parent = myRoot;
	}
	ZLTextTreeParagraph *tp = new ZLTextTreeParagraph(parent);
	addParagraphInternal(tp);
	return tp;
}

void ZLTextTreeModel::search(const std::string &text, std::size_t startIndex, std::size_t endIndex, bool ignoreCase) const {
	ZLTextModel::search(text, startIndex, endIndex, ignoreCase);
	for (std::vector<ZLTextMark>::const_iterator it = marks().begin(); it != marks().end(); ++it) {
		((ZLTextTreeParagraph*)(*this)[it->ParagraphIndex])->openTree();
	}
}

void ZLTextTreeModel::selectParagraph(std::size_t index) const {
	if (index < paragraphsNumber()) {
		ZLTextModel::selectParagraph(index);
		((ZLTextTreeParagraph*)(*this)[index])->openTree();
	}
}

ZLTextPlainModel::ZLTextPlainModel(const std::string &language, const std::size_t rowSize) : ZLTextModel(language, rowSize) {
}

void ZLTextPlainModel::createParagraph(ZLTextParagraph::Kind kind) {
	ZLTextParagraph *paragraph = (kind == ZLTextParagraph::TEXT_PARAGRAPH) ? new ZLTextParagraph() : new ZLTextSpecialParagraph(kind);
	addParagraphInternal(paragraph);
}

void ZLTextModel::addText(const std::string &text) {
	std::size_t len = text.length();
	if (myLastEntryStart != 0 && *myLastEntryStart == ZLTextParagraphEntry::TEXT_ENTRY) {
		std::size_t oldLen = 0;
		std::memcpy(&oldLen, myLastEntryStart + 1, sizeof(std::size_t));
		std::size_t newLen = oldLen + len;
		myLastEntryStart = myAllocator.reallocateLast(myLastEntryStart, newLen + sizeof(std::size_t) + 1);
		std::memcpy(myLastEntryStart + 1, &newLen, sizeof(std::size_t));
		std::memcpy(myLastEntryStart + sizeof(std::size_t) + 1 + oldLen, text.data(), len);
	} else {
		myLastEntryStart = myAllocator.allocate(len + sizeof(std::size_t) + 1);
		*myLastEntryStart = ZLTextParagraphEntry::TEXT_ENTRY;
		std::memcpy(myLastEntryStart + 1, &len, sizeof(std::size_t));
		std::memcpy(myLastEntryStart + sizeof(std::size_t) + 1, text.data(), len);
		myParagraphs.back()->addEntry(myLastEntryStart);
	}
}

void ZLTextModel::addText(const std::vector<std::string> &text) {
	if (text.size() == 0) {
		return;
	}
	std::size_t len = 0;
	for (std::vector<std::string>::const_iterator it = text.begin(); it != text.end(); ++it) {
		len += it->length();
	}
	if (myLastEntryStart != 0 && *myLastEntryStart == ZLTextParagraphEntry::TEXT_ENTRY) {
		std::size_t oldLen = 0;
		std::memcpy(&oldLen, myLastEntryStart + 1, sizeof(std::size_t));
		std::size_t newLen = oldLen + len;
		myLastEntryStart = myAllocator.reallocateLast(myLastEntryStart, newLen + sizeof(std::size_t) + 1);
		std::memcpy(myLastEntryStart + 1, &newLen, sizeof(std::size_t));
		std::size_t offset = sizeof(std::size_t) + 1 + oldLen;
		for (std::vector<std::string>::const_iterator it = text.begin(); it != text.end(); ++it) {
			std::memcpy(myLastEntryStart + offset, it->data(), it->length());
			offset += it->length();
		}
	} else {
		myLastEntryStart = myAllocator.allocate(len + sizeof(std::size_t) + 1);
		*myLastEntryStart = ZLTextParagraphEntry::TEXT_ENTRY;
		std::memcpy(myLastEntryStart + 1, &len, sizeof(std::size_t));
		std::size_t offset = sizeof(std::size_t) + 1;
		for (std::vector<std::string>::const_iterator it = text.begin(); it != text.end(); ++it) {
			std::memcpy(myLastEntryStart + offset, it->data(), it->length());
			offset += it->length();
		}
		myParagraphs.back()->addEntry(myLastEntryStart);
	}
}

void ZLTextModel::addFixedHSpace(unsigned char length) {
	myLastEntryStart = myAllocator.allocate(2);
	*myLastEntryStart = ZLTextParagraphEntry::FIXED_HSPACE_ENTRY;
	*(myLastEntryStart + 1) = length;
	myParagraphs.back()->addEntry(myLastEntryStart);
}

void ZLTextModel::addControl(ZLTextKind textKind, bool isStart) {
	myLastEntryStart = myAllocator.allocate(2);
	*myLastEntryStart = ZLTextParagraphEntry::CONTROL_ENTRY;
	*(myLastEntryStart + 1) = (textKind << 1) + (isStart ? 1 : 0);
	myParagraphs.back()->addEntry(myLastEntryStart);
}

void ZLTextModel::addStyleEntry(const ZLTextStyleEntry &entry) {
	int len = sizeof(unsigned short) + 4 + ZLTextStyleEntry::NUMBER_OF_LENGTHS * (sizeof(short) + 1);
	if (entry.isFeatureSupported(ZLTextStyleEntry::FONT_FAMILY)) {
		len += entry.fontFamily().length() + 1;
	}
	myLastEntryStart = myAllocator.allocate(len);
	char *address = myLastEntryStart;
	*address++ = entry.entryKind();
	std::memcpy(address, &entry.myFeatureMask, sizeof(unsigned short));
	address += sizeof(unsigned short);
	for (int i = 0; i < ZLTextStyleEntry::NUMBER_OF_LENGTHS; ++i) {
		*address++ = entry.myLengths[i].Unit;
		std::memcpy(address, &entry.myLengths[i].Size, sizeof(short));
		address += sizeof(short);
	}
	*address++ = entry.mySupportedFontModifier;
	*address++ = entry.myFontModifier;
	*address++ = entry.myAlignmentType;
	if (entry.isFeatureSupported(ZLTextStyleEntry::FONT_FAMILY)) {
		std::memcpy(address, entry.fontFamily().data(), entry.fontFamily().length());
		address += entry.fontFamily().length();
		*address++ = '\0';
	}
	myParagraphs.back()->addEntry(myLastEntryStart);
}

void ZLTextModel::addStyleCloseEntry() {
//TODO implement
//code below is from FBReaderJ
//	myLastEntryStart = myAllocator->allocate(2);
//	char *address = myLastEntryStart;

//	*address++ = ZLTextParagraphEntry::STYLE_CLOSE_ENTRY;
//	*address++ = 0;

//	myParagraphs.back()->addEntry(myLastEntryStart);
//	++myParagraphLengths.back();
}

void ZLTextModel::addHyperlinkControl(ZLTextKind textKind, ZLHyperlinkType hyperlinkType, const std::string &label) {
	myLastEntryStart = myAllocator.allocate(label.length() + 4);
	*myLastEntryStart = ZLTextParagraphEntry::HYPERLINK_CONTROL_ENTRY;
	*(myLastEntryStart + 1) = textKind;
	*(myLastEntryStart + 2) = hyperlinkType;
	std::memcpy(myLastEntryStart + 3, label.data(), label.length());
	*(myLastEntryStart + label.length() + 3) = '\0';
	myParagraphs.back()->addEntry(myLastEntryStart);
}

void ZLTextModel::addImage(const std::string &id, const ZLImageMap &imageMap, short vOffset) {
	myLastEntryStart = myAllocator.allocate(sizeof(const ZLImageMap*) + sizeof(short) + id.length() + 2);
	*myLastEntryStart = ZLTextParagraphEntry::IMAGE_ENTRY;
	const ZLImageMap *imageMapAddress = &imageMap;
	std::memcpy(myLastEntryStart + 1, &imageMapAddress, sizeof(const ZLImageMap*));
	std::memcpy(myLastEntryStart + 1 + sizeof(const ZLImageMap*), &vOffset, sizeof(short));
	std::memcpy(myLastEntryStart + 1 + sizeof(const ZLImageMap*) + sizeof(short), id.data(), id.length());
	*(myLastEntryStart + 1 + sizeof(const ZLImageMap*) + sizeof(short) + id.length()) = '\0';
	myParagraphs.back()->addEntry(myLastEntryStart);
}

void ZLTextModel::addBidiReset() {
	myLastEntryStart = myAllocator.allocate(1);
	*myLastEntryStart = ZLTextParagraphEntry::RESET_BIDI_ENTRY;
	myParagraphs.back()->addEntry(myLastEntryStart);
}
