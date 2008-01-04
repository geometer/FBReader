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

#ifndef __PLUCKERBOOKREADER_H__
#define __PLUCKERBOOKREADER_H__

#include <set>
#include <map>

#include <ZLEncodingConverter.h>

#include "../../bookmodel/BookReader.h"
#include "../EncodedTextReader.h"

class PluckerBookReader : public BookReader, public EncodedTextReader {

public:
	PluckerBookReader(const std::string &filePath, BookModel &model, const std::string &encoding);
	~PluckerBookReader();

	bool readDocument();

private:
	enum FontType {
		FT_REGULAR = 0,
		FT_H1 = 1,
		FT_H2 = 2,
		FT_H3 = 3,
		FT_H4 = 4,
		FT_H5 = 5,
		FT_H6 = 6,
		FT_BOLD = 7,
		FT_TT = 8,
		FT_SMALL = 9,
		FT_SUB = 10,
		FT_SUP = 11
	};

	void readRecord(size_t recordSize);
	void processTextRecord(size_t size, const std::vector<int> &pars);
	void processTextParagraph(char *start, char *end);
	void processTextFunction(char *ptr);
	void setFont(FontType font, bool start);
	void changeFont(FontType font);

	void safeAddControl(FBTextKind kind, bool start);
	void safeAddHyperlinkControl(const std::string &id);
	void safeBeginParagraph();
	void safeEndParagraph();

	void processHeader(FontType font, bool start);

private:
	std::string myFilePath;
	shared_ptr<ZLInputStream> myStream;
	FontType myFont;
	char *myCharBuffer;
	std::string myConvertedTextBuffer;
	bool myParagraphStarted;
	bool myBufferIsEmpty;
	ZLTextForcedControlEntry *myForcedEntry;
	std::vector<std::pair<FBTextKind,bool> > myDelayedControls;
	std::vector<std::string> myDelayedHyperlinks;
	unsigned short myCompressionVersion;
	unsigned char myBytesToSkip;

	std::set<std::pair<int, int> > myReferencedParagraphs;
	std::map<int, std::vector<int> > myParagraphMap;
	std::vector<int> *myParagraphVector;
	bool myParagraphStored;
};

#endif /* __PLUCKERBOOKREADER_H__ */
