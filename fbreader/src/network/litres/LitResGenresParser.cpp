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

#include "LitResGenresParser.h"


static const std::string TAG_GENRE = "genre";


LitResGenresParser::LitResGenresParser(std::map<std::string, LitResGenre> &genres) : myGenres(genres) {
	mySkipGenreClosing = false;
}

void LitResGenresParser::startElementHandler(const char *tag, const char **attributes) {
	if (TAG_GENRE == tag) {
		const char *id = attributeValue(attributes, "id");
		const char *title = attributeValue(attributes, "title");
		const char *token = attributeValue(attributes, "token");
		if (title != 0) {
			if (id != 0 && token != 0) {
				myGenres[std::string(token)] = LitResGenre(std::string(id), titlePrefix() + std::string(title));
				mySkipGenreClosing = true;
			} else {
				myTitleStack.push_back(std::string(title));
				myTitlePrefix = "";
			}
		}
	}
}

void LitResGenresParser::endElementHandler(const char *tag) {
	if (TAG_GENRE == tag) {
		if (!mySkipGenreClosing) {
			myTitleStack.pop_back();
			myTitlePrefix = "";
		}
		mySkipGenreClosing = false;
	}
}

void LitResGenresParser::characterDataHandler(const char *, size_t) {
}

const std::string &LitResGenresParser::titlePrefix() {
	if (myTitlePrefix.empty()) {
		for (std::vector<std::string>::const_iterator it = myTitleStack.begin(); it != myTitleStack.end(); ++it) {
			myTitlePrefix.append(*it);
			myTitlePrefix.append("/");
		}
	}
	return myTitlePrefix;
}

