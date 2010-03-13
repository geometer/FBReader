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

#ifndef __LITRESGENRESPARSER_H__
#define __LITRESGENRESPARSER_H__


#include <map>
#include <vector>
#include <string>

#include <ZLXMLReader.h>

class LitResGenre;

class LitResGenresParser : public ZLXMLReader {

public:
	LitResGenresParser(std::vector<shared_ptr<LitResGenre> > &genresTree, std::map<std::string, shared_ptr<LitResGenre> > &genresMap);

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, size_t len);

	void saveGenre(shared_ptr<LitResGenre> genre, const std::string &token);

	const std::string &titlePrefix();

private:
	std::vector<shared_ptr<LitResGenre> > &myGenresTree;
	std::map<std::string, shared_ptr<LitResGenre> > &myGenresMap;
	std::vector<shared_ptr<LitResGenre> > myStack;
	bool myDontPopStack;

	std::vector<std::string> myTitleStack;
	std::string myTitlePrefix;
};

#endif /* __LITRESGENRESPARSER_H__ */
