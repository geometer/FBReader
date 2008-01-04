/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLInputStream.h>

#include "DocBookBookReader.h"

#include "../../bookmodel/BookModel.h"
#include "../../model/Paragraph.h"

DocBookBookReader::DocBookBookReader(BookModel &model) : BookReader(model) {
	setMainTextModel();

	myReadText = false;
}

void DocBookBookReader::characterDataHandler(const char *text, int len) {
	addDataToBuffer(text, len);
}
	
void DocBookBookReader::startElementHandler(int tag, const char **) {
	switch (tag) {
		case _SECT1:
			myReadText = true;
			pushKind(REGULAR);
			beginContentsParagraph();
			break;
		case _PARA:
			if (myReadText) {
				beginParagraph();
			}
			break;
		case _TITLE:
			enterTitle();
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
			endContentsParagraph();
			insertEndOfSectionParagraph();
			break;
		case _PARA:
			endParagraph();
			break;
		case _TITLE:
			endParagraph();
			popKind();
			endContentsParagraph();
			exitTitle();
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

void DocBookBookReader::readBook(shared_ptr<ZLInputStream> stream) {
	readDocument(stream);
}
