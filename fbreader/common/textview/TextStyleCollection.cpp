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

#include "TextStyle.h"

TextStyleCollection *TextStyleCollection::ourInstance = 0;

TextStyleCollection::TextStyleCollection() {
	myBaseStyle = new BaseTextStyle("georgia", 24);

	registerStyle(REGULAR, "Regular Paragraph", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, 30, 0, ALIGN_UNDEFINED, 0.0, true);
	registerStyle(TITLE, "Title", 10, B3_TRUE, B3_UNDEFINED, 2, 7, 0, 0, 0, 0, ALIGN_CENTER, 0.0, false);
	registerStyle(POEM_TITLE, "Poem Title", 2, B3_TRUE, B3_UNDEFINED, 6, 6, 40, 0, 0, 0, ALIGN_UNDEFINED, 0.0, false);
	registerStyle(SECTION_TITLE, "Section Title", 6, B3_TRUE, B3_UNDEFINED, 0, 5, 0, 0, 0, 0, ALIGN_CENTER, 0.0, false);
	registerStyle(ANNOTATION, "Annotation", -2, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, 30, 0, ALIGN_UNDEFINED, 0.0, true);
	registerStyle(EPIGRAPH, "Epigraph", -2, B3_UNDEFINED, B3_TRUE, 0, 0, 80, 0, 0, 0, ALIGN_UNDEFINED, 0.0, true);
	registerStyle(SUBTITLE, "Subtitle", 0, B3_TRUE, B3_UNDEFINED, 0, 0, 0, 0, 0, 0, ALIGN_UNDEFINED, 0.0, true);
	registerStyle(CITE, "Cite", 0, B3_UNDEFINED, B3_TRUE, 0, 0, 0, 0, 0, 0, ALIGN_UNDEFINED, 0.0, true);
	registerStyle(AUTHOR, "Author", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 20, 0, 0, 0, ALIGN_UNDEFINED, 0.0, false);
	registerStyle(DATE, "Date", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 40, 0, 0, 0, ALIGN_UNDEFINED, 0.0, false);
	registerStyle(STANZA, "Stanza", 0, B3_UNDEFINED, B3_UNDEFINED, 6, 6, 0, 0, 0, 0, ALIGN_LEFT, 0.0, false);
	registerStyle(VERSE, "Verse", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 20, 0, 0, 0, ALIGN_LEFT, 0.0, false);
	registerStyle(IMAGE, "Image", 0, B3_UNDEFINED, B3_UNDEFINED, 8, 0, 0, 0, 0, 0, ALIGN_CENTER, 0.0, false);
	registerStyle(CONTENTS_TABLE_ENTRY, "Contents Table", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 7, 30, 0, -30, 0, ALIGN_LEFT, 0.0, false);
	registerStyle(LIBRARY_AUTHOR_ENTRY, "Author in Library", -2, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, 0, 0, ALIGN_LEFT, 1.2, false);
	registerStyle(LIBRARY_BOOK_ENTRY, "Book in Library", -2, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, 0, 0, ALIGN_LEFT, 1.2, false);

	registerStyle(FOOTNOTE, "Footnote", -6, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, 0, 10, ALIGN_UNDEFINED, 0.0, true);
	registerStyle(EMPHASIS, "Emphasis", 0, B3_UNDEFINED, B3_TRUE, 0, 0, 0, 0, 0, 0, ALIGN_UNDEFINED, 0.0, true);
	registerStyle(STRONG, "Strong", 0, B3_TRUE, B3_UNDEFINED, 0, 0, 0, 0, 0, 0, ALIGN_UNDEFINED, 0.0, true);
	registerStyle(SUB, "Subscript", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, 0, 0, ALIGN_UNDEFINED, 0.0, false);
	registerStyle(SUP, "Superscript", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, 0, 0, ALIGN_UNDEFINED, 0.0, false);
	registerStyle(CODE, "Code", 0, B3_UNDEFINED, B3_TRUE, 0, 0, 0, 0, 0, 0, ALIGN_UNDEFINED, 0.0, false);
	registerStyle(STRIKETHROUGH, "StrikeThrough", 0, B3_UNDEFINED, B3_UNDEFINED, 0, 0, 0, 0, 0, 0, ALIGN_UNDEFINED, 0.0, true);
}

TextStyleCollection::~TextStyleCollection() {
	delete myBaseStyle;
	for (std::map<TextKind,TextStyleDecoration*>::iterator it = myDecorationMap.begin(); it != myDecorationMap.end(); it++) {
		delete it->second;
	}
}

void TextStyleCollection::registerStyle(TextKind kind, const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShift, AlignmentType alignment, double lineSpace, bool allowHyphenations) {
	myDecorationMap[kind] = new TextStyleDecoration(name, fontSizeDelta, bold, italic, spaceBefore, spaceAfter, leftIndent, rightIndent, firstLineIndentDelta, verticalShift, alignment, lineSpace, allowHyphenations);
}

const TextStyleDecoration *TextStyleCollection::decoration(TextKind kind) const {
	std::map<TextKind,TextStyleDecoration*>::const_iterator it = myDecorationMap.find(kind);
	return (it != myDecorationMap.end()) ? it->second : 0;
}
