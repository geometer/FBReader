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

#ifndef __DOCBOOKREADER_H__
#define __DOCBOOKREADER_H__

#include <ZLFile.h>

#include "../../bookmodel/BookReader.h"

#include "OleStreamReader.h"

class DocBookReader : public OleStreamReader {

public:
	DocBookReader(BookModel &model, const std::string &encoding);
	~DocBookReader();
	bool readBook();

private:
	bool readDocument(shared_ptr<ZLInputStream> stream, size_t streamSize);
	//void parapgraphHandler(std::string paragraph);

	void handleChar(ZLUnicodeUtil::Ucs2Char ucs2char);
	void handleHardLinebreak();
	void handleParagraphEnd();
	void handlePageBreak();
	void handleTableSeparator();
	void handleTableEndRow();
	void handleFootNoteMark();
	void handleStartField();
	void handleSeparatorField();
	void handleEndField();
	void handleStartOfHeading();
	void handleOtherControlChar(ZLUnicodeUtil::Ucs2Char ucs2char);

	//formatting:
	void handleFontStyle(unsigned int fontStyle);
	void handleParagraphStyle(const OleMainStream::StyleInfo& styleInfo);

private:
	BookReader myModelReader;

	ZLUnicodeUtil::Ucs2String myFieldInfoBuffer;
	bool myFieldReading;
	bool myHyperlinkInserted;

	//formatting
	std::vector<FBTextKind> myKindStack;
	shared_ptr<ZLTextStyleEntry> myCurStyleEntry;
};

inline DocBookReader::~DocBookReader() {}

#endif /* __DOCBOOKREADER_H__ */
