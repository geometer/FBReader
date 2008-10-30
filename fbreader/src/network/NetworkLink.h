/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __NETWORKLINK_H__
#define __NETWORKLINK_H__

#include <string>
#include <vector>

#include <shared_ptr.h>
#include <ZLOptions.h>

#include "NetworkBookInfo.h"

class CurlData;

class NetworkLink {

protected:
	static std::string htmlEncode(const std::string &stringToEncode);

protected:
	NetworkLink(const std::string &siteName, const std::string &optionName);

public:
	virtual ~NetworkLink();

public:
	const std::string SiteName;
	ZLBooleanOption OnOption;

public:
	virtual shared_ptr<CurlData> simpleSearchData(NetworkBookList &books, const std::string &pattern) = 0;
	virtual shared_ptr<CurlData> advancedSearchData(NetworkBookList &books, const std::string &title, const std::string &author, const std::string &series, const std::string &tag, const std::string &annotation) = 0;
};

class NetworkLinkCollection {

public:
	static NetworkLinkCollection &instance();

private:
	static NetworkLinkCollection *ourInstance;

private:
	static bool downloadFile(const std::string &url, const std::string &fileName);

public:
	ZLStringOption DirectoryOption;

private:
	NetworkLinkCollection();
	~NetworkLinkCollection();

public:
	bool downloadBook(const NetworkBookInfo &book, NetworkBookInfo::URLType format, const std::string &fileName);

	bool simpleSearch(NetworkBookList &books, const std::string &pattern);
	bool advancedSearch(NetworkBookList &books, const std::string &title, const std::string &author, const std::string &series, const std::string &tag, const std::string &annotation);

	size_t size() const;
	NetworkLink &link(size_t index) const;

private:
	bool perform(const std::vector<shared_ptr<CurlData> > &dataList);

private:
	std::vector<shared_ptr<NetworkLink> > myLinks;
};

#endif /* __NETWORKLINK_H__ */
