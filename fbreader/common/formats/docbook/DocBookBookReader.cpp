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

#include "DocBookBookReader.h"

#include "../../bookmodel/BookModel.h"
#include "../../model/Paragraph.h"

DocBookBookReader::DocBookBookReader(BookModel &model) : BookReader(model) {
	setMainTextModel();

	myReadText = false;
	myInsideTitle = false;
}

void DocBookBookReader::characterDataHandler(const char *text, int len) {
	if (myCurrentParagraph != 0) {
		myBuffer.push_back(std::string());
		myBuffer.back().append(text, len);
	}
}
	
void DocBookBookReader::startElementHandler(int tag, const char **) {
	switch (tag) {
		case _SECT1:
			myReadText = true;
			pushKind(REGULAR);
			myCurrentContentsParagraph = new ParagraphWithReference();
			myCurrentContentsParagraph->addControl(CONTENTS_TABLE_ENTRY, true);
			break;
		case _PARA:
			if (myReadText) {
				beginParagraph();
			}
			break;
		case _TITLE:
			myInsideTitle = true;
			pushKind(SECTION_TITLE);
			if (myReadText) {
				beginParagraph();
			}
			break;
		case _EMPHASIS:
			addControl(EMPHASIS, true);
			break;
		case _CITETITLE:
			addControl(CITE, true);
			break;
		case _ULINK:
		case _EMAIL:
			addControl(CODE, true);
			break;
		case _BLOCKQUOTE:
			pushKind(STRONG);
			break;
		default:
			break;
	}
}

void DocBookBookReader::endElementHandler(int tag) {
	switch (tag) {
		case _SECT1:
			myReadText = false;
			popKind();
			addContentsParagraphToModel();
			myCurrentContentsParagraph = 0;
			mySectionContainsRegularContents = true;
			insertEndOfSectionParagraph();
			break;
		case _PARA:
			endParagraph();
			break;
		case _TITLE:
			endParagraph();
			popKind();
			addContentsParagraphToModel();
			myCurrentContentsParagraph = 0;
			myInsideTitle = false;
			break;
		case _EMPHASIS:
			addControl(EMPHASIS, false);
			break;
		case _CITETITLE:
			addControl(CITE, false);
			break;
		case _ULINK:
		case _EMAIL:
			addControl(CODE, false);
			break;
		case _BLOCKQUOTE:
			popKind();
			break;
		default:
			break;
	}
}

void DocBookBookReader::readBook(ZLInputStream &stream) {
	readDocument(stream);
}
