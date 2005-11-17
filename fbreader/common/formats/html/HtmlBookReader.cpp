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

#include <cctype>

#include <abstract/ZLFileImage.h>

#include "../txt/PlainTextFormat.h"
#include "HtmlBookReader.h"
#include "../../bookmodel/BookModel.h"

HtmlBookReader::HtmlBookReader(const std::string &baseDirectoryPath, BookModel &model, const PlainTextFormat &format, const std::string &encoding) : HtmlReader(encoding), BookReader(model), myBaseDirPath(baseDirectoryPath), myFormat(format) {
}

void HtmlBookReader::addConvertedDataToBuffer(const char *text, int len) {
	if (len > 0) {
		myConverter->convert(myConverterBuffer, text, text + len);
		addDataToBuffer(myConverterBuffer);
		myConverterBuffer.erase();
	}
}

bool HtmlBookReader::tagHandler(HtmlTag tag) {
	myConverter->reset();
	switch(tag.Code) {
		case _BODY:
			break;
		case _IMAGE:
			endParagraph();
			for (unsigned int i = 0; i < tag.Attributes.size(); i++) {
				if (tag.Attributes[i].Name == "SRC") {
					std::string fileName = tag.Attributes[i].Value;
					addImageReference(fileName);
					ZLImage *image = new ZLFileImage("image/auto", myBaseDirPath + fileName, 0);
					addImage(fileName, image);
				}
			}
			beginParagraph();
			break;
		// 9. text
		case _EM:
			addControl(EMPHASIS, tag.Start);
			break;
		case _STRONG:
			addControl(STRONG, tag.Start);
			break;
		case _DFN:
			//TODO: implement
			break;
		case _CODE:
			addControl(CODE, tag.Start);
			break;
		case _SAMP:
			//TODO: implement
			break;
		case _KBD:
			//TODO: implement
			break;
		case _VAR:
			//TODO: implement
			break;
		case _CITE:
			addControl(CITE, tag.Start);
			break;
		case _ABBR:
			//TODO: implement
			break;
		case _ACRONYM:
			//TODO: implement
			break;
		case _BLOCKQUOTE:
			//TODO: implement
			break;
		case _Q:
			//TODO: implement
			break;
		case _SUB:
			addControl(SUB, tag.Start);
			break;
		case _SUP:
			addControl(SUP, tag.Start);
			break;
		case _DIV:
			if (!tag.Start) {
				endParagraph();
				beginParagraph();
			}
			break;
		case _P:
		case _BR:
			endParagraph();
			beginParagraph();
			break;
		case _PRE:
			endParagraph();
			myIsPreformatted = tag.Start;
			mySpaceCounter = -1;
			myBreakCounter = 0;
			if (myFormat.breakType() == PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE) {
				if (tag.Start) {
					pushKind(PREFORMATTED);
				} else {
					popKind();
				}
			}
			beginParagraph();
			break;
		case _INS:
			//TODO: implement
			break;
		case _DEL:
			//TODO: implement
			break;
		// 10. lists
		case _UL:
		case _OL:
			if (tag.Start) {
				myListNumStack.push((tag.Code == _UL) ? 0 : 1);
			} else if (!myListNumStack.empty()) {
				myListNumStack.pop();
			}
			break;
		case _LI:
			if (tag.Start) {
				endParagraph();
				beginParagraph();
				if (!myListNumStack.empty()) {
					//TODO: add spaces and number/bullet
					addConvertedDataToBuffer("&bull; ", 7);
				}
			}
			break;
		case _DT:
			if (tag.Start) {
				endParagraph();
				beginParagraph();
			}
			break;
		case _DL:
		case _DD:
			break;
		//
		case _H1:
		case _H2:
		case _H3:
		case _H4:
		case _H5:
		case _H6:
			endParagraph();
			if (tag.Start) {
				insertEndOfSectionParagraph();
				enterTitle();
				beginContentsParagraph();
				pushKind(SECTION_TITLE);
			} else {
				popKind();
				endContentsParagraph();
				exitTitle();
			}
			beginParagraph();
			break;
		case _TT:
			addControl(CODE, tag.Start);
			break;
		case _B:
			addControl(STRONG, tag.Start);
			break;
		case _I:
			addControl(EMPHASIS, tag.Start);
			break;
		case _HEAD:
		case _TITLE:
		case _STYLE:
			if (tag.Start) {
				myIgnoreDataCounter++;
			} else {
				myIgnoreDataCounter--;
			}
			break;
		case _SELECT:
		case _SCRIPT:
			if (tag.Start) {
				myIgnoreDataCounter++;
			} else {
				myIgnoreDataCounter--;
			}
			break;
		case _A:
			if (tag.Start) {
				for (unsigned int i = 0; i < tag.Attributes.size(); i++) {
					if (tag.Attributes[i].Name == "NAME") {
						addHyperlinkLabel(tag.Attributes[i].Value);
					} else if (!myIsHyperlink && (tag.Attributes[i].Name == "HREF")) {
						const std::string &value = tag.Attributes[i].Value;
						if (!value.empty() && (value[0] == '#')) {
							addHyperlinkControl(HYPERLINK, value.substr(1));
							myIsHyperlink = true;
						}
					}
				}
			} else if (myIsHyperlink) {
				addControl(HYPERLINK, false);
				myIsHyperlink = false;
			}
			break;
		case _UNKNOWN:
			break;
	}
	return true;
}

bool HtmlBookReader::characterDataHandler(const char *text, int len) {
	if (myIgnoreDataCounter == 0) {
		const char *start = text;
		const char *end = text + len;
		if (myIsPreformatted) {
			int breakType = myFormat.breakType();
			if (breakType & PlainTextFormat::BREAK_PARAGRAPH_AT_NEW_LINE) {
				for (const char *ptr = text; ptr != end; ptr++) {
					if (*ptr == '\n') {
						if (start < ptr) {
							addConvertedDataToBuffer(start, ptr - start);
						} else {
							addDataToBuffer(" ", 1);
						}
						endParagraph();
						beginParagraph();
						start = ptr + 1;
					}
				}
				addConvertedDataToBuffer(start, end - start);
			} else if (breakType & PlainTextFormat::BREAK_PARAGRAPH_AT_LINE_WITH_INDENT) {
				for (const char *ptr = text; ptr != end; ptr++) {
					if (isspace(*ptr)) {
						if (*ptr == '\n') {
							mySpaceCounter = 0;
						} else if (mySpaceCounter >= 0) {
							mySpaceCounter++;
						}
					} else {
						if (mySpaceCounter > myFormat.ignoredIndent()) {
							if (ptr - start > mySpaceCounter) {
								addConvertedDataToBuffer(start, ptr - start - mySpaceCounter);
								endParagraph();
								beginParagraph();
							}
							start = ptr;
						}
						mySpaceCounter = -1;
					}
				}
				mySpaceCounter = std::max(mySpaceCounter, 0);
				if (end - start > mySpaceCounter) {
					addConvertedDataToBuffer(start, end - start - mySpaceCounter);
				}
			} else if (breakType & PlainTextFormat::BREAK_PARAGRAPH_AT_EMPTY_LINE) {
				for (const char *ptr = start; ptr != end; ptr++) {
					if (isspace(*ptr)) {
						if (*ptr == '\n') {
							myBreakCounter++;
						}
					} else {
						if (myBreakCounter > 1) {
							addConvertedDataToBuffer(start, ptr - start);
							endParagraph();
							beginParagraph();
							start = ptr;
						}
						myBreakCounter = 0;
					}
				}
				addConvertedDataToBuffer(start, end - start);
			}
		} else {
			if (!myIsStarted) {
				for (; start != end; start++) {
					if (!isspace(*start)) {
						myIsStarted = true;
						break;
					}
				}
			}
			addConvertedDataToBuffer(start, end - start);
		}
	}
	return true;
}

void HtmlBookReader::startDocumentHandler() {
	setMainTextModel();
	pushKind(REGULAR);
	beginParagraph();
	myIgnoreDataCounter = 0;
	myIsPreformatted = false;
	myIsHyperlink = false;
	myIsStarted = false;

	mySpaceCounter = -1;
	myBreakCounter = 0;
}

void HtmlBookReader::endDocumentHandler() {
	endParagraph();
}
