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

#ifndef __OPDSLINK_H__
#define __OPDSLINK_H__

#include "../NetworkLink.h"

class OPDSLink : public NetworkLink {

private:
	class AdvancedSearch;

protected:
	static shared_ptr<ZLExecutionData> createNetworkData(const std::string &url, NetworkOperationData &result);

public:
	OPDSLink(
		const std::string &siteName,
		const std::string &catalogURL,
		const std::string &searchURL,
		const std::string &title,
		const std::string &summary,
		const std::string &iconName
	);
	~OPDSLink();
	void setupAdvancedSearch(
		const std::string &type,
		const std::string &titleParameter,
		const std::string &authorParameter,
		const std::string &tagParameter,
		const std::string &annotationParameter
	);

private:
	const std::string searchURL(const std::string &pattern) const;

	shared_ptr<NetworkLibraryItem> libraryItem();
	shared_ptr<ZLExecutionData> simpleSearchData(
		NetworkOperationData &result,
		const std::string &pattern);
	shared_ptr<ZLExecutionData> advancedSearchData(
		NetworkOperationData &result,
		const std::string &titleAndSeries,
		const std::string &author,
		const std::string &tag,
		const std::string &annotation);
	shared_ptr<ZLExecutionData> resume(NetworkOperationData &result);

friend class OPDSCatalogItem;

private:
	const std::string myCatalogURL;
	const std::string mySearchURL;
	const std::string myTitle;
	const std::string mySummary;
	const std::string myIconName;
	shared_ptr<AdvancedSearch> myAdvancedSearch;
};

#endif /* __OPDSLINK_H__ */
