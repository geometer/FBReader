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

#include <ZLStringUtil.h>

#include "LitResAuthorsParser.h"


static const std::string TAG_PERSONS = "catalit-persons";
static const std::string TAG_SUBJECT = "subject";
static const std::string TAG_TITLE = "title";
static const std::string TAG_MAIN = "main";
static const std::string TAG_FIRST_NAME = "first-name";
static const std::string TAG_MIDDLE_NAME = "middle-name";
static const std::string TAG_LAST_NAME = "last-name";


LitResAuthorsParser::LitResAuthorsParser(std::vector<LitResAuthor> &authors) : myAuthors(authors), myState(START) {
}

void LitResAuthorsParser::startElementHandler(const char *tag, const char **attributes) {
	switch (myState) {
	case START:
		if (TAG_PERSONS == tag) {
			myState = PERSONS;
		}
		break;
	case PERSONS:
		if (TAG_SUBJECT == tag) {
			myId.clear();
			myTitle.clear();
			myFirstName.clear();
			myMiddleName.clear();
			myLastName.clear();
			const char *id = attributeValue(attributes, "id");
			if (id != 0) {
				myId.assign(id);
			}
			myState = SUBJECT;
		}
		break;
	case SUBJECT:
		if (TAG_TITLE == tag) {
			myState = TITLE;
		} else if (TAG_FIRST_NAME == tag) {
			myState = FIRST;
		} else if (TAG_MIDDLE_NAME == tag) {
			myState = MIDDLE;
		} else if (TAG_LAST_NAME == tag) {
			myState = LAST;
		}
		break;
	case TITLE:
		if (TAG_MAIN == tag) {
			myState = TITLE_MAIN;
		}
		break;
	case TITLE_MAIN:
		break;
	case FIRST:
		break;
	case MIDDLE:
		break;
	case LAST:
		break;
	}
}

void LitResAuthorsParser::endElementHandler(const char *tag) {
	switch (myState) {
	case START:
		break;
	case PERSONS:
		if (TAG_PERSONS == tag) {
			myState = START;
		}
		break;
	case SUBJECT:
		if (TAG_SUBJECT == tag) {
			if (!myId.empty() && !myTitle.empty()) {
				myAuthors.push_back(LitResAuthor(myId, myTitle, myFirstName, myMiddleName, myLastName));
			}
			myState = PERSONS;
		}
		break;
	case TITLE:
		if (TAG_TITLE == tag) {
			myState = SUBJECT;
		}
		break;
	case TITLE_MAIN:
		if (TAG_MAIN == tag) {
			myState = TITLE;
		}
		break;
	case FIRST:
		if (TAG_FIRST_NAME == tag) {
			myState = SUBJECT;
		}
		break;
	case MIDDLE:
		if (TAG_MIDDLE_NAME == tag) {
			myState = SUBJECT;
		}
		break;
	case LAST:
		if (TAG_LAST_NAME == tag) {
			myState = SUBJECT;
		}
		break;
	}
}

void LitResAuthorsParser::characterDataHandler(const char *text, size_t len) {
	switch (myState) {
	case START:
		break;
	case PERSONS:
		break;
	case SUBJECT:
		break;
	case TITLE:
		break;
	case TITLE_MAIN:
		myTitle.append(text, len);
		break;
	case FIRST:
		myFirstName.append(text, len);
		break;
	case MIDDLE:
		myMiddleName.append(text, len);
		break;
	case LAST:
		myLastName.append(text, len);
		break;
	}
}

