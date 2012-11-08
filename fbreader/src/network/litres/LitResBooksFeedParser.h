/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __LITRESBOOKSFEEDPARSER_H__
#define __LITRESBOOKSFEEDPARSER_H__

#include <vector>
#include <map>

#include <ZLXMLReader.h>

#include "../NetworkItems.h"

#include "LitResBooksFeedItem.h"

class NetworkLink;
struct LitResGenre;
class NetworkAuthenticationManager;

class LitResBooksFeedParser : public ZLXMLReader {

public:
	LitResBooksFeedParser(const NetworkLink &link, NetworkItem::List &books, LitResBooksFeedItem::LoadingState *state = 0);

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, std::size_t len);

private:
	enum State {
		START, CATALOG, BOOK, BOOK_DESCRIPTION, HIDDEN, TITLE_INFO, 
		GENRE, AUTHOR, FIRST_NAME, MIDDLE_NAME, LAST_NAME, AUTHOR_ID, BOOK_TITLE,
		ANNOTATION, DATE, LANGUAGE,
	};

	std::string stringAttributeValue(const char **attributes, const char *name);
	void processState(const std::string &tag, bool closed, const char **attributes);
	State getNextState(const std::string &tag, bool closed);

private:
	const NetworkLink &myLink;

	NetworkItem::List &myBooks;
	std::string myBuffer;

	unsigned int myIndex;

	State myState;

	std::string myBookId;
	std::string myTitle;
	std::string mySummary;
	std::string myLanguage;
	std::string myDate;
	std::string mySeriesTitle;
	int myIndexInSeries;

	std::string myAuthorFirstName;
	std::string myAuthorMiddleName;
	std::string myAuthorLastName;
	std::string myAuthorId;
	std::vector<NetworkBookItem::AuthorData> myAuthors;
	std::vector<std::string> myAuthorsIds;

	std::vector<std::string> myTags;
	NetworkItem::UrlInfoCollection myURLByType;
	std::vector<shared_ptr<BookReference> > myReferences;

	LitResBooksFeedItem::LoadingState *myLoadingState;
};

#endif /* __LITRESBOOKSFEEDPARSER_H__ */
