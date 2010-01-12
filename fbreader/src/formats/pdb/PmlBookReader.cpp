/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLTextParagraph.h>
#include <ZLUnicodeUtil.h>
#include <ZLStringUtil.h>

#include "PmlBookReader.h"
#include "../../bookmodel/BookModel.h"

PmlBookReader::PmlBookReader(BookReader &bookReader, const PlainTextFormat&, const std::string &encoding) : PmlReader(encoding), myBookReader(bookReader) {
}

PmlBookReader::~PmlBookReader() {
}

bool PmlBookReader::readDocument(ZLInputStream& stream) {
    myBookReader.pushKind(REGULAR);
    myBookReader.beginParagraph();
	myParagraphIsEmpty = true;
    bool code = PmlReader::readDocument(stream);
    myBookReader.endParagraph();
	return code;
}

void PmlBookReader::addCharData(const char *data, size_t len, bool convert) {
	if (!myBookReader.paragraphIsOpen()) {
		myBookReader.beginParagraph();
	}
	static std::string newString;
	if (len != 0) {
		if (!myConverter.isNull() && convert) {
			myConverter->convert(newString, data, data + len);
		} else {
			newString.append(data, len);
		}
		if (myState.SmallCaps) {
			myBookReader.addData(ZLUnicodeUtil::toUpper(newString));
		} else {
			myBookReader.addData(newString);
		}
		newString.erase();
		if (myParagraphIsEmpty) {
			myParagraphIsEmpty = false;
		}
	}
}

void PmlBookReader::switchFontProperty(FontProperty property) {
	if (!myBookReader.paragraphIsOpen()) {
		myBookReader.beginParagraph();
	}
	switch (property) {
		case FONT_BOLD:
			if (myState.Bold) {
				myBookReader.pushKind(STRONG);
			} else {
				myBookReader.popKind();
			}
			myBookReader.addControl(STRONG, myState.Bold);
			break;
		case FONT_ITALIC:
			if (myState.Italic) {
				if (!myState.Bold) {
					myBookReader.pushKind(EMPHASIS);
					myBookReader.addControl(EMPHASIS, true);
				} else {
					myBookReader.popKind();
					myBookReader.addControl(STRONG, false);
	
					myBookReader.pushKind(EMPHASIS);
					myBookReader.addControl(EMPHASIS, true);
					myBookReader.pushKind(STRONG);
					myBookReader.addControl(STRONG, true);
				}
			} else {
				if (!myState.Bold) {
					myBookReader.addControl(EMPHASIS, false);
					myBookReader.popKind();
				} else {
					myBookReader.addControl(STRONG, false);
					myBookReader.popKind();
					myBookReader.addControl(EMPHASIS, false);
					myBookReader.popKind();
	
					myBookReader.pushKind(STRONG);
					myBookReader.addControl(STRONG, true);
				}
			}
			break;
		case FONT_UNDERLINED:
			break;
		case FONT_SUBSCRIPT: //don't have to be mixed with other style tags
			if (myState.Subscript) {
				myBookReader.pushKind(SUB);
			} else {
				myBookReader.popKind();
			}
			myBookReader.addControl(SUB, myState.Subscript);
			break;			
		case FONT_SUPERSCRIPT: //Should not be mixed with other style tags
			if (myState.Superscript) {
				myBookReader.pushKind(SUP);
			} else {
				myBookReader.popKind();
			}
			myBookReader.addControl(SUP, myState.Superscript);
			break;			
	}
}

void PmlBookReader::newLine() {
	if (myBookReader.paragraphIsOpen()) {
		myBookReader.endParagraph();
	}
	if (myParagraphIsEmpty)	{
		myBookReader.beginParagraph(ZLTextParagraph::EMPTY_LINE_PARAGRAPH);	
		myBookReader.endParagraph();
	} else {
		myParagraphIsEmpty = true;
	}
	newParagraph();	
}

void PmlBookReader::newPage() {
	if (myBookReader.paragraphIsOpen()) {
		myBookReader.endParagraph();
	}
	//newLine();
	newParagraph();
}

void PmlBookReader::newParagraph() {
	if (myBookReader.paragraphIsOpen()) {
		myBookReader.endParagraph();
	}
	myBookReader.beginParagraph();	
	if (myState.Alignment != ALIGN_UNDEFINED) {
        setAlignment();
    }
	if (myState.FontSize != NORMAL) {
		setFontSize();
	}
	if (myState.IndentBlockOn && (myState.Indent != 0)) {
		setIndent();
	}
}

void PmlBookReader::setAlignment() {
	ZLTextStyleEntry entry;
    entry.setAlignmentType(myState.Alignment);
    myBookReader.addControl(entry);
}

void PmlBookReader::setIndent() {
	ZLTextStyleEntry entry;
	entry.setLength(ZLTextStyleEntry::LENGTH_FIRST_LINE_INDENT_DELTA, 0, ZLTextStyleEntry::SIZE_UNIT_PERCENT);
	entry.setLength(ZLTextStyleEntry::LENGTH_LEFT_INDENT, (short)myState.Indent, ZLTextStyleEntry::SIZE_UNIT_PERCENT);
	myBookReader.addControl(entry);
}

void PmlBookReader::setFontSize() {
	if (!myBookReader.paragraphIsOpen()) {
		myBookReader.beginParagraph();
	}
	ZLTextStyleEntry entry;
    switch(myState.FontSize) {
		case SMALLER: 
			entry.setFontSizeMag(-1);
			break;
		case LARGER:
			entry.setFontSizeMag(1);
			break;
		default:
			break;
	}
    myBookReader.addControl(entry);
}

void PmlBookReader::addLink(FBTextKind kind, const std::string &id, bool on) {
	switch (kind) {
		case INTERNAL_HYPERLINK:
        case FOOTNOTE:
        //case EXTERNAL_HYPERLINK:
        //case BOOK_HYPERLINK:
			if (on) {
    			myBookReader.addHyperlinkControl(kind, id);
			} else {
    			myBookReader.addControl(kind, false);
			}
			break;
		default:
			break;
	}
}

void PmlBookReader::addLinkLabel(const std::string &label) {
	myBookReader.addHyperlinkLabel(label);
}

void PmlBookReader::addImageReference(const std::string &id) {
	const bool stopParagraph = myBookReader.paragraphIsOpen();
	if (stopParagraph) {
    	myBookReader.endParagraph();
	}
	myBookReader.addImageReference(id);
	if (stopParagraph) {
    	myBookReader.beginParagraph();
	}
}
