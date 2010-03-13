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

#include <ZLStringUtil.h>

#include "LitResGenresParser.h"

#include "LitResGenre.h"

static const std::string TAG_GENRE = "genre";


LitResGenresParser::LitResGenresParser(std::vector<shared_ptr<LitResGenre> > &genresTree, std::map<std::string, shared_ptr<LitResGenre> > &genresMap) :
	myGenresTree(genresTree), 
	myGenresMap(genresMap), 
	myDontPopStack(false) {
}

void LitResGenresParser::saveGenre(shared_ptr<LitResGenre> genre, const std::string &token) {
	if (myStack.empty()) {
		myGenresTree.push_back(genre);
	} else {
		myStack.back()->Children.push_back(genre);
	}
	if (genre->Id.empty()) {
		myStack.push_back(genre);
	} else {
		myDontPopStack = true;
		if (!token.empty()) {
			myGenresMap[token] = genre;
		}
	}
}

void LitResGenresParser::startElementHandler(const char *tag, const char **attributes) {
	if (TAG_GENRE == tag) {
		const char *id = attributeValue(attributes, "id");
		const char *title = attributeValue(attributes, "title");
		const char *token = attributeValue(attributes, "token");
		std::string strId, strTitle, strToken;
		if (id != 0) {
			strId = id;
		}
		if (title != 0) {
			strTitle = title;
		}
		if (token != 0) {
			strToken = token;
		}
		saveGenre(new LitResGenre(strId, strTitle), strToken);
	}
}

void LitResGenresParser::endElementHandler(const char *tag) {
	if (TAG_GENRE == tag) {
		if (!myDontPopStack) {
			myStack.pop_back();
		}
		myDontPopStack = false;
	}
}

void LitResGenresParser::characterDataHandler(const char *, size_t) {
}

