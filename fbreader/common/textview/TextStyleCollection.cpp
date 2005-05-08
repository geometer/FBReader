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

#include <abstract/ZLScreenSize.h>

#include "TextStyle.h"

static const std::string COLORS = "Colors";

ZLColorOption TextStyle::RegularTextColorOption(COLORS, "Text", ZLColor(0, 0, 0));
ZLColorOption TextStyle::SelectedTextColorOption(COLORS, "SelectedText", ZLColor(0, 0, 127));
ZLColorOption TextStyle::HyperlinkTextColorOption(COLORS, "Hyperlink", ZLColor(63, 63, 127));

TextStyleCollection *TextStyleCollection::ourInstance = NULL;

TextStyleCollection &TextStyleCollection::instance() {
	if (ourInstance == NULL) {
		ourInstance = new TextStyleCollection();
	}
	return *ourInstance;
}

void TextStyleCollection::deleteInstance() {
	if (ourInstance != NULL) {
		delete ourInstance;
		ourInstance = NULL;
	}
}

TextStyleCollection::TextStyleCollection() {
	int defaultFontSize = 24;
	int defaultParagraphIndent = 30;
	switch (ZLScreenSize::getSize()) {
		case ZLScreenSize::SIZE_DESKTOP:
			defaultFontSize = 16;
			defaultParagraphIndent = 20;
			break;
		case ZLScreenSize::SIZE_240x320:
		case ZLScreenSize::SIZE_320x320:
			defaultFontSize = 12;
			defaultParagraphIndent = 15;
			break;
		case ZLScreenSize::SIZE_640x480:
			defaultFontSize = 24;
			defaultParagraphIndent = 30;
			break;
	}
	myBaseStyle = new BaseTextStyle("georgia", defaultFontSize);

	registerStyle(REGULAR, "Regular Paragraph", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, defaultParagraphIndent, 0, ALIGN_UNDEFINED, 0.0, B3_TRUE, false);
	registerStyle(TITLE, "Title", 10, B3_TRUE, B3_UNDEFINED, 2, 7, 0, 0, 0, 0, ALIGN_CENTER, 0.0, B3_FALSE, false);
	registerStyle(POEM_TITLE, "Poem Title", 2, B3_TRUE, B3_UNDEFINED, 6, 6, 40, 0, 0, 0, ALIGN_UNDEFINED, 0.0, B3_FALSE, false);
	registerStyle(SECTION_TITLE, "Section Title", 6, B3_TRUE, B3_UNDEFINED, 0, 5, 0, 0, 0, 0, ALIGN_CENTER, 0.0, B3_FALSE, false);
	registerStyle(ANNOTATION, "Annotation", -2, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, defaultParagraphIndent, 0, ALIGN_UNDEFINED, 0.0, B3_TRUE, false);
	registerStyle(EPIGRAPH, "Epigraph", -2, B3_UNDEFINED, B3_TRUE, 0, 0, 80, 0, 0, 0, ALIGN_UNDEFINED, 0.0, B3_TRUE, false);
	registerStyle(SUBTITLE, "Subtitle", 0, B3_TRUE, B3_UNDEFINED, 0, 0, 0, 0, 0, 0, ALIGN_UNDEFINED, 0.0, B3_TRUE, false);
	registerStyle(AUTHOR, "Author", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 20, 0, 0, 0, ALIGN_UNDEFINED, 0.0, B3_FALSE, false);
	registerStyle(DATE, "Date", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 40, 0, 0, 0, ALIGN_UNDEFINED, 0.0, B3_FALSE, false);
	registerStyle(STANZA, "Stanza", 0, B3_UNDEFINED, B3_UNDEFINED, 6, 6, 0, 0, 0, 0, ALIGN_LEFT, 0.0, B3_FALSE, false);
	registerStyle(VERSE, "Verse", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 20, 0, 0, 0, ALIGN_LEFT, 0.0, B3_FALSE, false);
	registerStyle(IMAGE, "Image", 0, B3_UNDEFINED, B3_UNDEFINED, 8, 0, 0, 0, 0, 0, ALIGN_CENTER, 0.0, B3_FALSE, false);
	registerStyle(CONTENTS_TABLE_ENTRY, "Contents Table", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 7, defaultParagraphIndent, 0, -defaultParagraphIndent, 0, ALIGN_LEFT, 0.0, B3_FALSE, false);
	registerStyle(LIBRARY_AUTHOR_ENTRY, "Author in Library", -2, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, 0, 0, ALIGN_LEFT, 1.2, B3_FALSE, false);
	registerStyle(LIBRARY_BOOK_ENTRY, "Book in Library", -2, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, 0, 0, ALIGN_LEFT, 1.2, B3_FALSE, false);
	registerStyle(PREFORMATTED, "Preformatted text", 0, B3_UNDEFINED, B3_TRUE, 0, 0, 0, 0, 0, 0, ALIGN_LEFT, 0.0, B3_FALSE, false);

	registerStyle(CITE, "Cite", 0, B3_UNDEFINED, B3_TRUE, 0, B3_UNDEFINED, false);
	registerStyle(HYPERLINK, "Hyperlink", 0, B3_UNDEFINED, B3_UNDEFINED, 0, B3_FALSE, true);
	registerStyle(FOOTNOTE, "Footnote", -6, B3_UNDEFINED, B3_UNDEFINED, 10, B3_FALSE, true);
	registerStyle(EMPHASIS, "Emphasis", 0, B3_UNDEFINED, B3_TRUE, 0, B3_UNDEFINED, false);
	registerStyle(STRONG, "Strong", 0, B3_TRUE, B3_UNDEFINED, 0, B3_UNDEFINED, false);
	registerStyle(SUB, "Subscript", -4, B3_UNDEFINED, B3_UNDEFINED, -4, B3_FALSE, false);
	registerStyle(SUP, "Superscript", -4, B3_UNDEFINED, B3_UNDEFINED, 10, B3_FALSE, false);
	registerStyle(CODE, "Code", 0, B3_UNDEFINED, B3_TRUE, 0, B3_FALSE, false);
	registerStyle(STRIKETHROUGH, "StrikeThrough", 0, B3_UNDEFINED, B3_UNDEFINED, 0, B3_UNDEFINED, false);
}

TextStyleCollection::~TextStyleCollection() {
	delete myBaseStyle;
	for (std::map<TextKind,TextStyleDecoration*>::iterator it = myDecorationMap.begin(); it != myDecorationMap.end(); it++) {
		delete (*it).second;
	}
}

void TextStyleCollection::registerStyle(TextKind kind, const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShift, AlignmentType alignment, double lineSpace, Boolean3 allowHyphenations, bool isHyperlink) {
	FullTextStyleDecoration *decoration = new FullTextStyleDecoration(name, fontSizeDelta, bold, italic, spaceBefore, spaceAfter, leftIndent, rightIndent, firstLineIndentDelta, verticalShift, alignment, lineSpace, allowHyphenations);
	if (isHyperlink) {
		decoration->setHyperlinkStyle();
	}
	myDecorationMap.insert(std::pair<TextKind,TextStyleDecoration*>(kind, decoration));
}

void TextStyleCollection::registerStyle(TextKind kind, const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int verticalShift, Boolean3 allowHyphenations, bool isHyperlink) {
	TextStyleDecoration *decoration = new TextStyleDecoration(name, fontSizeDelta, bold, italic, verticalShift, allowHyphenations);
	if (isHyperlink) {
		decoration->setHyperlinkStyle();
	}
	myDecorationMap.insert(std::pair<TextKind,TextStyleDecoration*>(kind, decoration));
}

const TextStyleDecoration *TextStyleCollection::decoration(TextKind kind) const {
	std::map<TextKind,TextStyleDecoration*>::const_iterator it = myDecorationMap.find(kind);
	return (it != myDecorationMap.end()) ? (*it).second : 0;
}
