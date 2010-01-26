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

#ifndef __LITRESUTIL_H__
#define __LITRESUTIL_H__

#include <string>
#include <map>

#include <shared_ptr.h>

class LitResGenre;

class LitResUtil {

public:
	static const std::string LFROM;
	static const std::string CURRENCY_SUFFIX;

private:
	static shared_ptr<LitResUtil> ourInstance;

	LitResUtil();

public:
	static LitResUtil &Instance();

public:
	static std::string appendLFrom(const std::string &url);
	static std::string litresLink(const std::string &path);
	static void makeDemoUrl(std::string &url, const std::string &bookId);

	const std::map<std::string, shared_ptr<LitResGenre> > &genresMap();
	const std::vector<shared_ptr<LitResGenre> > &genresTree();
	const std::map<shared_ptr<LitResGenre>, std::string> &genresTitles();

	void fillGenreIds(const std::string &tag, std::vector<std::string> &ids);

private:
	void validateGenres();
	bool loadGenres();
	void buildGenresTitles(const std::vector<shared_ptr<LitResGenre> > &genres, const std::string &titlePrefix = "");

private:
	std::vector<shared_ptr<LitResGenre> > myGenresTree;
	std::map<std::string, shared_ptr<LitResGenre> > myGenresMap;
	std::map<shared_ptr<LitResGenre>, std::string> myGenresTitles;
	bool myGenresValid;
};

#endif /* __LITRESUTIL_H__ */
