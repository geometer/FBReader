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

void HtmlBookReader::endParagraph() {
	myConverter.convertBuffer(myBuffer);
	BookReader::endParagraph();
}

bool HtmlBookReader::tagHandler(HtmlTag tag) {
	switch(tag.Code) {
		case _BODY:
			break;
		case _P:
		case _BR:
			endParagraph();
			beginParagraph();
			break;
		case _LI:
			//TODO: add bullet
			endParagraph();
			beginParagraph();
			break;
		case _TITLE:
			endParagraph();
			if (tag.Start) {
				pushKind(TITLE);
			} else {
				popKind();
			}
			beginParagraph();
			break;
		case _H1:
		case _H2:
		case _H3:
		case _H4:
		case _H5:
		case _H6:
			endParagraph();
			if (tag.Start) {
				pushKind(SECTION_TITLE);
			} else {
				popKind();
			}
			beginParagraph();
			break;
		case _TT:
			addControl(CODE, tag.Start);
			break;
		case _STRONG:
		case _B:
			addControl(STRONG, tag.Start);
			break;
		case _I:
			addControl(EMPHASIS, tag.Start);
			break;
		case _SUB:
			addControl(SUB, tag.Start);
			break;
		case _SUP:
			addControl(SUP, tag.Start);
			break;
		case _CITE:
			addControl(CITE, tag.Start);
			break;
		case _STYLE:
			myIgnoreData = tag.Start;
			break;
		case _UNKNOWN:
			break;
	}
	return true;
}

bool HtmlBookReader::characterDataHandler(const char *text, int len) {
	if ((len > 0) && !myIgnoreData) {
		myBuffer.push_back(std::string());
		myBuffer.back().append(text, len);
	}
	return true;
}

void HtmlBookReader::startDocumentHandler() {
	setMainTextModel();
	pushKind(REGULAR);
	beginParagraph();
	myIgnoreData = false;
}

void HtmlBookReader::endDocumentHandler() {
	endParagraph();
}
