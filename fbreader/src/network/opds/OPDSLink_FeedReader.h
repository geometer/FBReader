/*
 * Copyright (C) 2008-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __OPDSLINK_FEEDREADER_H__
#define __OPDSLINK_FEEDREADER_H__

#include <map>
#include <string>

#include "OPDSFeedReader.h"
#include "OPDSLink.h"

class OPDSLink::FeedReader : public OPDSFeedReader {

public:
	FeedReader(
		shared_ptr<NetworkLink>& link,
		const std::string &baseURL
	);

public:
	void processFeedEntry(shared_ptr<OPDSEntry> entry);
	void processFeedStart();
	void processFeedMetadata(shared_ptr<OPDSFeedMetadata> feed);
	void processFeedEnd();

private:
	const std::string myBaseURL;
	shared_ptr<NetworkLink>& myLink;
};

#endif /* __OPDSLINK_FEEDREADER_H__ */
