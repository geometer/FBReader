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

#ifndef __LITRESAUTHORSPARSER_H__
#define __LITRESAUTHORSPARSER_H__

#include <vector>
#include <map>

#include <ZLXMLReader.h>

#include "../../NetworkItems.h"

class NetworkLink;
struct LitResGenre;
class NetworkAuthenticationManager;

class LitResAuthorsParser : public ZLXMLReader {

public:
	LitResAuthorsParser(const NetworkLink &link, const std::map<NetworkItem::URLType,std::string> &urlByType, NetworkItem::List &authors);

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, size_t len);

private:
	enum State {
		START, CATALOG, SUBJECT, TITLE, MAIN, FIRST_NAME, MIDDLE_NAME, LAST_NAME
	};

	std::string stringAttributeValue(const char **attributes, const char *name);
	void processState(const std::string &tag, bool closed, const char **attributes);
	State getNextState(const std::string &tag, bool closed);

	std::string getSubtitle() const;

private:
	const NetworkLink &myLink;
	const std::map<NetworkItem::URLType,std::string> myUrlByType;

	NetworkItem::List &myAuthors;
	std::string myBuffer;

	State myState;

	std::string myAuthorId;
	std::string myAuthorDisplayName;
	std::string myAuthorFirstName;
	std::string myAuthorMiddleName;
	std::string myAuthorLastName;
};

#endif /* __LITRESAUTHORSPARSER_H__ */
