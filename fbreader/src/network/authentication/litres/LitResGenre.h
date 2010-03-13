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

#ifndef __LITRESGENRE_H__
#define __LITRESGENRE_H__

#include <string>
#include <vector>
#include <map>

#include <shared_ptr.h>

struct LitResGenre {
	std::string Id;
	std::string Title;
	std::vector<shared_ptr<LitResGenre> > Children;

	LitResGenre();
	LitResGenre(const std::string &id, const std::string &title);
};

class LitResGenreMap {

public:
	static const LitResGenreMap &Instance();

private:
	static LitResGenreMap *ourInstance;

private:
	LitResGenreMap();

public:
	const std::map<std::string, shared_ptr<LitResGenre> > &genresMap() const;
	const std::vector<shared_ptr<LitResGenre> > &genresTree() const;
	const std::map<shared_ptr<LitResGenre>, std::string> &genresTitles() const;
	void fillGenreIds(const std::string &tag, std::vector<std::string> &ids) const;

private:
	void validateGenres() const;
	bool loadGenres() const;
	void buildGenresTitles(const std::vector<shared_ptr<LitResGenre> > &genres, const std::string &titlePrefix = "") const;

	mutable std::vector<shared_ptr<LitResGenre> > myGenresTree;
	mutable std::map<std::string, shared_ptr<LitResGenre> > myGenresMap;
	mutable std::map<shared_ptr<LitResGenre>, std::string> myGenresTitles;
	mutable bool myInitialized;
};

#endif /* __LITRESGENRE_H__ */
