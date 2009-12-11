/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __LITRESDATAPARSER_H__
#define __LITRESDATAPARSER_H__

#include <map>
#include <set>

#include <ZLXMLReader.h>

#include "../NetworkLibraryItems.h"

class LitResGenre;
class NetworkAuthenticationManager;


class LitResDataParser : public ZLXMLReader {

public:
	LitResDataParser(NetworkLibraryItemList &books, shared_ptr<NetworkAuthenticationManager> mgr);

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, size_t len);

private:
	enum State {
		START, CATALOG, BOOK, BOOK_DESCRIPTION, HIDDEN, TITLE_INFO, 
		GENRE, AUTHOR, FIRST_NAME, MIDDLE_NAME, LAST_NAME, BOOK_TITLE, 
		ANNOTATION, DATE, LANGUAGE,
	};

	void processState(const std::string &tag, bool closed);
	State getNextState(const std::string &tag, bool closed);

	NetworkLibraryBookItem &currentBook();

private:
	NetworkLibraryItemList &myBooks;
	std::string myBuffer;
	shared_ptr<NetworkLibraryItem> myCurrentBook;
	unsigned int myIndex;

	State myState;
	std::map<std::string, std::string> myAttributes;

	std::string myAuthorFirstName;
	std::string myAuthorMiddleName;
	std::string myAuthorLastName;

	shared_ptr<NetworkAuthenticationManager> myAuthenticationManager;
};

inline NetworkLibraryBookItem &LitResDataParser::currentBook() { return (NetworkLibraryBookItem &) *myCurrentBook; }

#endif /* __LITRESDATAPARSER_H__ */
