/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __BOOKREADER_H__
#define __BOOKREADER_H__

#include <vector>
#include <stack>
#include <string>

#include "../model/TextKind.h"
#include "../model/Paragraph.h"

class BookModel;
class TextModel;
class PlainTextModel;
class ZLInputStream;

class BookReader {

public:
	BookReader(BookModel &model);
	virtual ~BookReader();

	void setMainTextModel();
	void setFootnoteTextModel(const std::string &id);
	void unsetTextModel();

	void insertEndOfSectionParagraph();
	void insertEndOfTextParagraph();
	void pushKind(TextKind kind);
	bool popKind();
	void beginParagraph(Paragraph::Kind kind = Paragraph::TEXT_PARAGRAPH);
	void endParagraph();
	bool paragraphIsOpen() const;
	void addControl(TextKind kind, bool start);
	void addControl(const ForcedControlEntry &entry);
	void addHyperlinkControl(TextKind kind, const std::string &label);
	void addHyperlinkLabel(const std::string &label);
	void addHyperlinkLabel(const std::string &label, int paragraphNumber);
	void addFixedHSpace(unsigned char length);

	void addImageReference(const std::string &id, short vOffset = 0);
	void addImage(const std::string &id, shared_ptr<const ZLImage> image);

	void beginContentsParagraph(int referenceNumber = -1);
	void endContentsParagraph();
	void setReference(size_t contentsParagraphNumber, int referenceNumber);

	void addData(const std::string &data);
	void addContentsData(const std::string &data);

	void enterTitle() { myInsideTitle = true; }
	void exitTitle() { myInsideTitle = false; }

	const BookModel &model() const { return myModel; }

	void reset();

private:
	void insertEndParagraph(Paragraph::Kind kind);
	void flushTextBufferToParagraph();

private:
	BookModel &myModel;
	shared_ptr<TextModel> myCurrentTextModel;

	std::vector<TextKind> myKindStack;

	bool myTextParagraphExists;
	std::stack<TreeParagraph*> myTOCStack;
	bool myLastTOCParagraphIsEmpty;

	bool mySectionContainsRegularContents;
	bool myInsideTitle;

	std::vector<std::string> myBuffer;
	std::vector<std::string> myContentsBuffer;
};

inline bool BookReader::paragraphIsOpen() const {
	return myTextParagraphExists;
}

#endif /* __BOOKREADER_H__ */
