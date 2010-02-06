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

#ifndef __LITRESLINK_H__
#define __LITRESLINK_H__

#include <map>
#include <string>

#include "../NetworkLink.h"

class NetworkAuthenticationManager;
class LitResGenre;

class LitResLink : public NetworkLink {

public:
	static const std::string CURRENCY_SUFFIX;

public:
	LitResLink();

private:
	shared_ptr<ZLExecutionData> simpleSearchData(NetworkOperationData &result, const std::string &pattern) const;
	shared_ptr<ZLExecutionData> advancedSearchData(NetworkOperationData &result, const std::string &titleAndSeries, const std::string &author, const std::string &tag, const std::string &annotation) const;

	shared_ptr<NetworkLibraryItem> libraryItem() const;

public:
	shared_ptr<NetworkAuthenticationManager> authenticationManager() const;

	void rewriteUrl(std::string &url) const;
	std::string litresUrl(const std::string &path) const;

	const std::map<std::string, shared_ptr<LitResGenre> > &genresMap() const;
	const std::vector<shared_ptr<LitResGenre> > &genresTree() const;
	const std::map<shared_ptr<LitResGenre>, std::string> &genresTitles() const;

	void fillGenreIds(const std::string &tag, std::vector<std::string> &ids) const;

private:
	void validateGenres() const;
	bool loadGenres() const;
	void buildGenresTitles(const std::vector<shared_ptr<LitResGenre> > &genres, const std::string &titlePrefix = "") const;

private:
	shared_ptr<NetworkAuthenticationManager> myAuthenticationManager;

	mutable std::vector<shared_ptr<LitResGenre> > myGenresTree;
	mutable std::map<std::string, shared_ptr<LitResGenre> > myGenresMap;
	mutable std::map<shared_ptr<LitResGenre>, std::string> myGenresTitles;
	mutable bool myGenresValid;
};

#endif /* __LITRESLINK_H__ */
