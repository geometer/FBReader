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

#include "HtmlBookReader.h"
#include "../../bookmodel/BookModel.h"

HtmlBookReader::HtmlBookReader(BookModel &model) : BookReader(model) {
}

void HtmlBookReader::flushTextBufferToParagraph() {
	if (myIsPreformatted) {
		std::string fullText;
		ZLStringUtil::append(fullText, myBuffer);
		myBuffer.clear();
		int index = -1;
		do {
			//TODO: process '\t' and ' ' symbols
			int oldIndex = index + 1;
			index = fullText.find('\n', oldIndex);
			int len = ((index == -1) ? fullText.length() : index) - oldIndex;
			if (len > 0) {
				addDataToBuffer(fullText.data() + oldIndex, len);
			} else {
				addDataToBuffer(" ");
			}
			myIsPreformatted = false;
			endParagraph();
			beginParagraph();
			myIsPreformatted = true;
		} while (index != -1);
	} else {
		if (!myIsStarted) {
			for (std::vector<std::string>::const_iterator it = myBuffer.begin(); !myIsStarted && (it != myBuffer.end()); it++) {
				for (std::string::const_iterator jt = it->begin(); jt != it->end(); jt++) {
					if (!isspace(*jt)) {
						myIsStarted = true;
						break;
					}
				}
			}
		}
		if (myIsStarted) {
			myConverter->convertBuffer(myBuffer);
			BookReader::flushTextBufferToParagraph();
		}
	}
}

bool HtmlBookReader::tagHandler(HtmlTag tag) {
	switch(tag.Code) {
		case _BODY:
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
		case _P:
		case _BR:
			endParagraph();
			beginParagraph();
			break;
		case _PRE:
			endParagraph();
			myIsPreformatted = tag.Start;
			if (tag.Start) {
				pushKind(PREFORMATTED);
			} else {
				popKind();
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
					addDataToBuffer("&bull; ");
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
				myInsideTitle = true;
				beginContentsParagraph();
				pushKind(SECTION_TITLE);
			} else {
				popKind();
				endContentsParagraph();
				myInsideTitle = false;
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
		addDataToBuffer(text, len);
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
}

void HtmlBookReader::endDocumentHandler() {
	endParagraph();
}
