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

#include "FB2BookReader.h"
#include "../../bookmodel/BookModel.h"
#include "../../model/Paragraph.h"
#include "../../model/Image.h"

static const struct {
	FB2Reader::TagCode code;
	TextKind kind;
} CONTROLS[] = {
	{FB2Reader::_SUB, SUB},	
	{FB2Reader::_SUP, SUP},	
	{FB2Reader::_CODE, CODE},	
	{FB2Reader::_STRIKETHROUGH, STRIKETHROUGH},
	{FB2Reader::_STRONG, STRONG},
	{FB2Reader::_EMPHASIS, EMPHASIS},
	{FB2Reader::_A, FOOTNOTE},
	{FB2Reader::_UNKNOWN, (TextKind)-1}
};

static TextKind control(int code) {
	for (int i = 0; ; i++) {
		if ((CONTROLS[i].code == code) || (CONTROLS[i].code == FB2Reader::_UNKNOWN)) {
			return CONTROLS[i].kind;
		}
	}
};

static const struct {
	FB2Reader::TagCode code;
	TextKind kind;
} PARAGRAPHS[] = {
	{FB2Reader::_V, VERSE},	
	{FB2Reader::_SUBTITLE, SUBTITLE},	
	{FB2Reader::_TEXT_AUTHOR, AUTHOR},	
	{FB2Reader::_DATE, DATE},
	{FB2Reader::_UNKNOWN, (TextKind)-1}
};

FB2BookReader::FB2BookReader(BookModel &model) : BookReader(model) {
	myInsidePoem = false;
	mySectionDepth = 0;
	myBodyCounter = 0;
}

void FB2BookReader::characterDataHandler(const char *text, int len) {
	addDataToBuffer(text, len);
}

const char *attributeValue(const char **xmlattributes, const char *name) {
	while (*xmlattributes != 0) {
		bool useNext = strcmp(*xmlattributes, name) == 0;
		xmlattributes++;
		if (*xmlattributes == 0) {
			return 0;
		}
		if (useNext) {
			return *xmlattributes;
		}
		xmlattributes++;
	}
	return 0;
}

const char *attributeValueByPostfix(const char **xmlattributes, const char *postfix) {
	int postfixLen = strlen(postfix);
	while (*xmlattributes != 0) {
		int len = strlen(*xmlattributes);
		bool useNext = (len >= postfixLen) && (strcmp((*xmlattributes) + len - postfixLen, postfix) == 0);
		xmlattributes++;
		if (*xmlattributes == 0) {
			return 0;
		}
		if (useNext) {
			return *xmlattributes;
		}
		xmlattributes++;
	}
	return 0;
}

const char *reference(const char **xmlattributes) {
	const char *ref = attributeValueByPostfix(xmlattributes, ":href");
	return ((ref != 0) && (*ref == '#')) ? ref + 1 : 0;
}
	
void FB2BookReader::startElementHandler(int tag, const char **xmlattributes) {
	const char *id = attributeValue(xmlattributes, "id");
	if (id != 0) {
		if (myBodyCounter > 1) {
			setFootnoteTextModel(id);
		} else {
			addHyperlinkLabel(id);
		}
	}
	switch (tag) {
		case _P:
			beginParagraph();
			break;
		case _V:
		case _SUBTITLE:
		case _TEXT_AUTHOR:
		case _DATE:
			for (int i = 0; PARAGRAPHS[i].code != _UNKNOWN; i++) {
				if (tag == PARAGRAPHS[i].code) {
					pushKind(PARAGRAPHS[i].kind);
					beginParagraph();
					break;
				}
			}
			break;
		case _CITE:
			pushKind(CITE);
			break;
		case _SECTION:
			insertEndOfSectionParagraph();
			endContentsParagraph();
			mySectionDepth++;
			beginContentsParagraph();
			break;
		case _TITLE:
			myInsideTitle = true;
			if (myInsidePoem) {
				pushKind(POEM_TITLE);
			} else if (mySectionDepth == 0) {
				insertEndOfSectionParagraph();
				pushKind(TITLE);
			} else {
				pushKind(SECTION_TITLE);
			}
			break;
		case _POEM:
			myInsidePoem = true;
			break;
		case _STANZA:
			pushKind(STANZA);
			beginParagraph(Paragraph::BEFORE_SKIP_PARAGRAPH);
			endParagraph();
			break;
		case _EPIGRAPH:
			pushKind(EPIGRAPH);
			break;
		case _ANNOTATION:
			if (myBodyCounter == 0) {
				setMainTextModel();
			}
			pushKind(ANNOTATION);
			break;
		case _SUB:
		case _SUP:
		case _CODE:
		case _STRIKETHROUGH:
		case _STRONG:
		case _EMPHASIS:
			addControl(control(tag), true);
			break;
		case _A:
		{
			const char *ref = reference(xmlattributes);
			if (ref != 0) {
				addHyperlinkControl(control(tag), ref);
			} else {
				addControl(control(tag), true);
			}
			break;
		}
		case _IMAGE:
			if (!currentTextModelIsNull()) {
				const char *ref = reference(xmlattributes);
				if (ref != 0) {
					addImageToParagraph(ref);
				}
			}
			break;
		case _BINARY:
		{
			const char *contentType = attributeValue(xmlattributes, "content-type");
			const char *id = attributeValue(xmlattributes, "id");
			if ((contentType != 0) && (id != 0)) {
				myCurrentImage = new Image(contentType);
				addImageToModel(id, myCurrentImage);
			}
			break;
		}
		case _EMPTY_LINE:
			beginParagraph(Paragraph::EMPTY_LINE_PARAGRAPH);
			endParagraph();
			break;
		case _BODY:
			myBodyCounter++;
			if (myBodyCounter == 1) {
				setMainTextModel();
			}
			pushKind(REGULAR);
			break;
		default:
			break;
	}
}

void FB2BookReader::endElementHandler(int tag) {
	switch (tag) {
		case _P:
			endParagraph();
			break;
		case _V:
		case _SUBTITLE:
		case _TEXT_AUTHOR:
		case _DATE:
			popKind();
			endParagraph();
			break;
		case _CITE:
			popKind();
			break;
		case _SECTION:
			if (myBodyCounter > 1) {
				unsetTextModel();
			}
			mySectionDepth--;
			endContentsParagraph();
			break;
		case _TITLE:
			myInsideTitle = false;
			popKind();
			break;
		case _POEM:
			myInsidePoem = false;
			break;
		case _STANZA:
			beginParagraph(Paragraph::AFTER_SKIP_PARAGRAPH);
			endParagraph();
			popKind();
			break;
		case _EPIGRAPH:
			popKind();
			break;
		case _ANNOTATION:
			popKind();
			if (myBodyCounter == 0) {
				insertEndOfSectionParagraph();
				unsetTextModel();
			}
			break;
		case _SUB:
		case _SUP:
		case _CODE:
		case _STRIKETHROUGH:
		case _STRONG:
		case _EMPHASIS:
		case _A:
			addControl(control(tag), false);
			break;
		case _BINARY:
			flushTextBufferToImage();
			myCurrentImage = 0;
			break;
		case _BODY:
			popKind();
			unsetTextModel();
			break;
		default:
			break;
	}
}

void FB2BookReader::readBook(ZLInputStream &stream) {
	readDocument(stream);
}
