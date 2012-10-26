/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __NETWORKLINKCOLLECTION_H__
#define __NETWORKLINKCOLLECTION_H__

#include <string>
#include <vector>

#include <shared_ptr.h>

#include <ZLOptions.h>
#include <ZLNetworkRequest.h>

#include "NetworkItems.h"

class NetworkLink;
class NetworkBookCollection;
class BookReference;

class ZLNetworkSSLCertificate;

class NetworkLinkCollection {

private:
	class Comparator;

public:
	static NetworkLinkCollection &Instance();

private:
	static NetworkLinkCollection *ourInstance;

public:
	ZLStringOption DirectoryOption;
	ZLIntegerOption LastUpdateTimeOption;

private:
	NetworkLinkCollection();
	~NetworkLinkCollection();

public:
	void initialize();

	std::string bookFileName(const BookReference &reference);

	bool downloadBook(const BookReference &reference, std::string &fileName, const ZLNetworkSSLCertificate &sslSertificate, shared_ptr<ZLNetworkRequest::Listener> listener);

	shared_ptr<NetworkBookCollection> simpleSearch(const std::string &pattern);
	shared_ptr<NetworkBookCollection> advancedSearch(const std::string &titleAndSeries, const std::string &author, const std::string &tag, const std::string &annotation);

	size_t size() const;
	size_t numberOfEnabledLinks() const;
	NetworkLink &link(size_t index) const;

	const std::string &errorMessage() const;

	void rewriteUrl(std::string &url, bool externalUrl = false) const;

	//void deleteLink(NetworkLink& link);
	//void saveLink(NetworkLink& link, bool isAuto = false);

private:
	void synchronize();

	std::string bookFileName(const std::string &url, BookReference::Format format, BookReference::Type type);

	void updateLinks(std::string genericUrl);
	shared_ptr<ZLFile> getGenericFile(std::string genericUrl);
	void addOrUpdateLink(shared_ptr<NetworkLink> link);

private:
	typedef std::vector<shared_ptr<NetworkLink> > LinkVector;
	LinkVector myLinks;
	std::string myErrorMessage;
	bool myIsInitialized;

friend class NetworkLinksUpdater;
};

inline const std::string &NetworkLinkCollection::errorMessage() const { return myErrorMessage; }

#endif /* __NETWORKLINKCOLLECTION_H__ */
