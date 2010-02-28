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

class LitResLink : public NetworkLink {

public:
	static const std::string CURRENCY_SUFFIX;

public:
	LitResLink();

private:
	shared_ptr<ZLExecutionData> simpleSearchData(NetworkOperationData &result, const std::string &pattern) const;
	shared_ptr<ZLExecutionData> advancedSearchData(NetworkOperationData &result, const std::string &titleAndSeries, const std::string &author, const std::string &tag, const std::string &annotation) const;

	shared_ptr<NetworkItem> libraryItem() const;

public:
	shared_ptr<NetworkAuthenticationManager> authenticationManager() const;

	void rewriteUrl(std::string &url, bool isUrlExternal = false) const;

private:
	shared_ptr<NetworkAuthenticationManager> myAuthenticationManager;
};

#endif /* __LITRESLINK_H__ */
