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

#ifndef __OPDSFEEDREADER_H__
#define __OPDSFEEDREADER_H__

#include "OPDSMetadata.h"


class OPDSFeedReader {

public:
	OPDSFeedReader() {}
	virtual ~OPDSFeedReader() {}

public:
	virtual void processFeedEntry(shared_ptr<OPDSEntry> entry) = 0;
	virtual void processFeedStart() = 0;
	virtual void processFeedMetadata(shared_ptr<OPDSFeedMetadata> feed) = 0;
	virtual void processFeedEnd() = 0;
};


#endif /* __OPDSFEEDREADER_H__ */
