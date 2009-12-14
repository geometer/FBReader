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

#ifndef __NETWORKOPDSFEEDREADER_H__
#define __NETWORKOPDSFEEDREADER_H__

#include <set>
#include <string>

#include "OPDSFeedReader.h"

class NetworkOperationData;

class NetworkLibraryItem;


class NetworkOPDSFeedReader : public OPDSFeedReader {

public:
	NetworkOPDSFeedReader(const std::string &baseURL, NetworkOperationData &result, const std::set<std::string> &ignoredFeeds);

public:
	void processFeedEntry(shared_ptr<OPDSEntry> entry);
	void processFeedStart();
	void processFeedMetadata(shared_ptr<OPDSFeedMetadata> feed);
	void processFeedEnd();

private:
	shared_ptr<NetworkLibraryItem> readBookItem(OPDSEntry &entry);
	shared_ptr<NetworkLibraryItem> readCatalogItem(OPDSEntry &entry);

private:
	const std::string myBaseURL;
	NetworkOperationData &myData;
	unsigned int myIndex;
	const std::set<std::string> myIgnoredFeeds;
};


#endif /* __NETWORKOPDSFEEDREADER_H__ */
