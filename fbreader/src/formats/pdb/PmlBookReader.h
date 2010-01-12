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

#ifndef __PMLBOOKREADER_H__
#define __PMLBOOKREADER_H__

#include <string>

#include "PmlReader.h"
#include "../../bookmodel/BookReader.h"
#include "../txt/PlainTextFormat.h"

class PmlBookReader : public PmlReader {

public:
	PmlBookReader(BookReader &bookReader, const PlainTextFormat &format, const std::string &encoding);
	~PmlBookReader();
	
	bool readDocument(ZLInputStream &stream);

protected:
	void addCharData(const char *data, size_t len, bool convert);
	void addLink(FBTextKind kind, const std::string &id, bool on);
	void addLinkLabel(const std::string &label);
	void addImageReference(const std::string &id);
	void switchFontProperty(FontProperty property);
	void setFontSize();
	void newLine();
	void newPage();
	void newParagraph();

private:
	void setAlignment();
	void setIndent();

private:
	BookReader& myBookReader;
	bool myParagraphIsEmpty;

	/*FontType myFont;
	char *myCharBuffer;
	std::string myConvertedTextBuffer;
	bool myParagraphStarted;
	bool myBufferIsEmpty;
	ZLTextStyleEntry *myForcedEntry;
	std::vector<std::pair<FBTextKind,bool> > myDelayedControls;
	std::vector<std::string> myDelayedHyperlinks;
	unsigned short myCompressionVersion;
	unsigned char myBytesToSkip;

	std::set<std::pair<int, int> > myReferencedParagraphs;
	std::map<int, std::vector<int> > myParagraphMap;
	std::vector<int> *myParagraphVector;
	bool myParagraphStored;*/
};

#endif /* __PMLBOOKREADER_H__ */
