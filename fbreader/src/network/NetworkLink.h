/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#include "NetworkLibraryItems.h"

class ZLExecutionData;

struct NetworkOperationData;
class NetworkAuthenticationManager;

class NetworkLink {

public:
	static std::string NetworkDataDirectory();
	static std::string CertificatesPathPrefix();

protected:
	NetworkLink(const std::string &siteName, const std::string &title);

public:
	virtual ~NetworkLink();

public:
	const std::string SiteName;
	const std::string Title;
	ZLBooleanOption OnOption;

public:
	virtual shared_ptr<ZLExecutionData> simpleSearchData(NetworkOperationData &data, const std::string &pattern) const = 0;
	virtual shared_ptr<ZLExecutionData> advancedSearchData(NetworkOperationData &data, const std::string &titleAndSeries, const std::string &author, const std::string &tag, const std::string &annotation) const = 0;
	virtual shared_ptr<ZLExecutionData> resume(NetworkOperationData &data) const;

	virtual shared_ptr<NetworkAuthenticationManager> authenticationManager() const = 0;
	virtual shared_ptr<NetworkLibraryItem> libraryItem() const = 0;

	virtual void rewriteUrl(std::string &url, bool isUrlExternal = false) const = 0;

private: // disable copying
	NetworkLink(const NetworkLink &);
	const NetworkLink &operator = (const NetworkLink &);
};

#endif /* __NETWORKLINK_H__ */
