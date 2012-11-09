/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __MOBIPOCKETHTMLBOOKREADER_H__
#define __MOBIPOCKETHTMLBOOKREADER_H__

#include <set>

#include "../html/HtmlBookReader.h"

class MobipocketHtmlBookReader : public HtmlBookReader {

public:
	MobipocketHtmlBookReader(const ZLFile &file, BookModel &model, const PlainTextFormat &format, const std::string &encoding);
	void readDocument(ZLInputStream &stream);

private:
	void startDocumentHandler();
	bool tagHandler(const HtmlTag &tag);
	bool characterDataHandler(const char *text, std::size_t len, bool convert);
	shared_ptr<HtmlTagAction> createAction(const std::string &tag);

public:
	class TOCReader {
	
	public:
		TOCReader(MobipocketHtmlBookReader &reader);
		void reset();

		void addReference(std::size_t position, const std::string &text);

		void setStartOffset(std::size_t position);
		void setEndOffset(std::size_t position);

		bool rangeContainsPosition(std::size_t position);

		void startReadEntry(std::size_t position);
		void endReadEntry();
		void appendText(const char *text, std::size_t len);

		const std::map<std::size_t,std::string> &entries() const;

	private:	
		MobipocketHtmlBookReader &myReader;

		std::map<std::size_t,std::string> myEntries;

		bool myIsActive;
		std::size_t myStartOffset;
		std::size_t myEndOffset;

		std::size_t myCurrentReference;
		std::string myCurrentEntryText;
	};

private:
	int myImageCounter;
	const std::string myFileName;

	std::vector<std::pair<std::size_t,std::size_t> > myPositionToParagraphMap;
	std::set<std::size_t> myFileposReferences;
	bool myInsideGuide;
	TOCReader myTocReader;

friend class MobipocketHtmlImageTagAction;
friend class MobipocketHtmlHrefTagAction;
friend class MobipocketHtmlGuideTagAction;
friend class MobipocketHtmlReferenceTagAction;
friend class MobipocketHtmlPagebreakTagAction;
friend class TOCReader;
};

#endif /* __MOBIPOCKETHTMLBOOKREADER_H__ */
