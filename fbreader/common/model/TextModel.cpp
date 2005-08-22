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

void TextModel::search(const std::string &text, bool ignoreCase) const {
	ZLSearchPattern pattern(text, ignoreCase);
	myMarks.clear();

	for (std::vector<Paragraph*>::const_iterator it = myParagraphs.begin(); it != myParagraphs.end(); it++) {
		int offset = 0;
		const std::vector<ParagraphEntry*> &entries = ((Paragraph*)*it)->entries();
		for (std::vector<ParagraphEntry*>::const_iterator jt = entries.begin(); jt != entries.end(); jt++) {
			if ((*jt)->entryKind() == ParagraphEntry::TEXT_ENTRY) {
				const ZLString &str = ((TextEntry*)*jt)->text();
				for (int pos = ZLSearchUtil::find(str, pattern); pos != -1; pos = ZLSearchUtil::find(str, pattern, pos + 1)) {
					myMarks.push_back(TextMark(it - myParagraphs.begin(), offset + pos, pattern.length()));
				}
				offset += str.length();
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

TreeParagraph *TreeModel::createParagraph(TreeParagraph *parent) {
	if (parent == 0) {
		parent = myRoot;
	}
	TreeParagraph *tp = new TreeParagraph(parent);
	addParagraphInternal(tp);
	return tp;
}

void TreeModel::search(const std::string &text, bool ignoreCase) const {
	TextModel::search(text, ignoreCase);
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
