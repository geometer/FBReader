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

#ifndef __BOOKREADER_H__
#define __BOOKREADER_H__

#include <vector>
#include <string>

#include "../model/TextKind.h"
#include "../model/Paragraph.h"

class BookModel;
class PlainTextModel;
class Image;
class ZLInputStream;

class BookReader {

protected:
	BookReader(BookModel &model);

public:
	virtual ~BookReader() {}

protected:
	void setMainTextModel();
	void setFootnoteTextModel(const std::string &id);
	void unsetTextModel();
	bool currentTextModelIsNull() const;

	void insertEndOfSectionParagraph();
	void pushKind(TextKind kind);
	void popKind();
	void beginParagraph(Paragraph::Kind kind = Paragraph::TEXT_PARAGRAPH);
	void endParagraph();
	void addControl(TextKind kind, bool start);
	void addHyperlinkControl(TextKind kind, const std::string &label);
	void addHyperlinkLabel(const std::string &label);

	void addImageToParagraph(const std::string &id);
	void addImageToModel(const std::string &id, Image *image);

	void beginContentsParagraph();
	void endContentsParagraph();

	void addDataToBuffer(const std::string &data);
	void addDataToBuffer(const char *data, int len);
	virtual void flushTextBufferToParagraph();
	void flushTextBufferToImage();

private:
	BookModel &myModel;
	PlainTextModel *myCurrentTextModel;

	std::vector<TextKind> myKindStack;

	Paragraph *myCurrentParagraph;
	ParagraphWithReference *myCurrentContentsParagraph;

	bool mySectionContainsRegularContents;

protected:
	std::vector<std::string> myBuffer;

	Image *myCurrentImage;

	bool myInsideTitle;
};

#endif /* __BOOKREADER_H__ */
