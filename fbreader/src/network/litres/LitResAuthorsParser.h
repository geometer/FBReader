/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLXMLReader.h>

#include "LitResAuthor.h"


class LitResAuthorsParser : public ZLXMLReader {

public:
	LitResAuthorsParser(std::vector<LitResAuthor> &authors);

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, size_t len);

	enum State {
		START, PERSONS, SUBJECT, TITLE, TITLE_MAIN, FIRST, MIDDLE, LAST
	};

private:
	std::vector<LitResAuthor> &myAuthors;
	std::string myId, myTitle, myFirstName, myMiddleName, myLastName;
	State myState;
};

#endif /* __LITRESAUTHORSPARSER_H__ */
