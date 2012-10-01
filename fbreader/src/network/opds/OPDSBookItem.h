/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __OPDSBOOKITEM_H__
#define __OPDSBOOKITEM_H__

#include "OPDSLink.h"
#include "OPDSMetadata.h"

class OPDSBookItem : public NetworkBookItem {

public:
	OPDSBookItem(const OPDSLink &networkLink, OPDSEntry &entry, std::string baseUrl, unsigned int index);

public:
	static BookReference::Format formatByZLMimeType(const std::string &mimeType);
	static BookReference::Type typeByRelation(const std::string &rel);

protected:
	static std::string getDate(OPDSEntry &entry);
	static std::vector<AuthorData> getAuthors(OPDSEntry &entry);
	static std::vector<std::string> getTags(OPDSEntry &entry);
	static UrlInfoCollection getUrls(const OPDSLink &networkLink, OPDSEntry &entry, std::string baseUrl);
	static std::vector<shared_ptr<BookReference> > getReferences(const OPDSLink &networkLink, OPDSEntry &entry, std::string baseUrl);
};

#endif /* __OPDSBOOKITEM_H__ */
