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

#include <cctype>
#include <algorithm>

#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLResource.h>
#include <ZLNetworkManager.h>
#include <ZLNetworkUtil.h>
#include <ZLibrary.h>
#include "../fbreader/FBReader.h"

#include "NetworkLinkCollection.h"

#include "../options/FBCategoryKey.h"

#include "../database/booksdb/BooksDB.h"

#include "NetworkOperationData.h"
#include "NetworkBookCollection.h"
#include "BookReference.h"

#include "opds/OPDSLink.h"
#include "opds/OPDSLink_GenericFeedReader.h"
#include "opds/OPDSLink_FeedReader.h"
#include "opds/OPDSXMLParser.h"

#include "opds/URLRewritingRule.h"

NetworkLinkCollection *NetworkLinkCollection::ourInstance = 0;

NetworkLinkCollection &NetworkLinkCollection::Instance() {
	if (ourInstance == 0) {
		ourInstance = new NetworkLinkCollection();
	}
	return *ourInstance;
}

class NetworkLinkCollection::Comparator {

public:
	bool operator() (
		const shared_ptr<NetworkLink> &first,
		const shared_ptr<NetworkLink> &second
	) const;

private:
	std::string removeLeadingNonAscii(const std::string &title) const;
};

std::string NetworkLinkCollection::Comparator::removeLeadingNonAscii(const std::string &title) const {
	std::string str = title;
	std::string::iterator it = str.begin();
	for (; it != str.end(); ++it) {
		if (((*it & 0x80) == 0) && isalnum(*it)) {
			break;
		}
	}
	if (it != str.end()) {
		str.erase(str.begin(), it);
	}
	return str;
}

bool NetworkLinkCollection::Comparator::operator() (
	const shared_ptr<NetworkLink> &first,
	const shared_ptr<NetworkLink> &second
) const {
	return
		removeLeadingNonAscii(first->SiteName) <
		removeLeadingNonAscii(second->SiteName);
}

void NetworkLinkCollection::deleteLink(NetworkLink& link) {
	if (myLocks.count(link.SiteName)) {
		pthread_mutex_lock(&*(myLocks[link.SiteName]));
	}
	BooksDB::Instance().deleteNetworkLink(link.SiteName);
	for (std::vector<shared_ptr<NetworkLink> >::iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		if (&(**it) == &link) {
			myLinks.erase(it);
			break;
		}
	}
	if (myLocks.count(link.SiteName)) {
		myExists[link.SiteName] = false;
		pthread_mutex_unlock(&*(myLocks[link.SiteName]));
	}
	FBReader::Instance().invalidateNetworkView();
	FBReader::Instance().refreshWindow();
}

void NetworkLinkCollection::saveLink(NetworkLink& link, bool isAuto) {
	saveLinkWithoutRefreshing(link, isAuto);
	FBReader::Instance().refreshWindow();
}

void NetworkLinkCollection::saveLinkWithoutRefreshing(NetworkLink& link, bool isAuto) {
	bool found = false;
	bool updated = false;
	for (std::vector<shared_ptr<NetworkLink> >::iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		if (&(**it) == &link) {
			found = true;
			updated = true;
			break;
		} else if ((**it).SiteName == link.SiteName) {
			if (link.getPredefinedId() != std::string()) {
				if (*(link.getUpdated()) > *((**it).getUpdated())) {
					(*it)->loadFrom(link);
					updated = true;
				}
			} else if (isAuto) {
				if (*(link.getUpdated()) > *((**it).getUpdated())) {
					(*it)->loadLinksFrom(link);
					updated = true;
				}
			} else {
				(*it)->loadSummaryFrom(link);
				updated = true;
			}
			found = true;
			break;
		}
	}
	if (!found) {
		shared_ptr<NetworkLink> newlink = new OPDSLink(link.SiteName);
		newlink->loadFrom(link);
		myLinks.push_back(newlink);
		std::sort(myLinks.begin(), myLinks.end(), Comparator());
		updated = true;
	}
	if (updated) {
		BooksDB::Instance().saveNetworkLink(link, isAuto);
		FBReader::Instance().invalidateNetworkView();
	}
}

void *updLinks( void *ptr ) {
	NetworkLinkCollection* nc = (NetworkLinkCollection*) ptr;
	std::string genericUrl = nc->myGenericUrl;
	std::vector<shared_ptr<NetworkLink> > links;
	shared_ptr<OPDSFeedReader> fr = new OPDSLink::GenericFeedReader(links);
	shared_ptr<ZLXMLReader> prsr = new OPDSXMLParser(fr);
	ZLNetworkManager::Instance().perform(ZLNetworkManager::Instance().createXMLParserRequest(genericUrl, prsr));
	for (std::vector<shared_ptr<NetworkLink> >::iterator it = links.begin(); it != links.end(); ++it) {
		nc->saveLinkWithoutRefreshing(**it, true);
	}
	for (std::vector<shared_ptr<NetworkLink> >::iterator it = nc->myTempCustomLinks.begin(); it != nc->myTempCustomLinks.end(); ++it) {
		shared_ptr<NetworkLink> link = 0;
		std::string url = (*it)->url(NetworkLink::URL_MAIN);
		shared_ptr<OPDSFeedReader> fr = new OPDSLink::FeedReader(link, url);
		shared_ptr<ZLXMLReader> prsr = new OPDSXMLParser(fr);
		ZLNetworkManager::Instance().perform(ZLNetworkManager::Instance().createXMLParserRequest(url, prsr));
		if (link != 0) {
			if (nc->myLocks[link->SiteName] != 0 && pthread_mutex_trylock(&*(nc->myLocks[link->SiteName])) == 0) {//TODO: why it can be 0?
				if (nc->myExists[link->SiteName]) {
					nc->saveLinkWithoutRefreshing(*link, true);
				}
				pthread_mutex_unlock(&*(nc->myLocks[link->SiteName]));
			}
		}
	}
}

NetworkLinkCollection::NetworkLinkCollection() :
	DirectoryOption(ZLCategoryKey::NETWORK, "Options", "DownloadDirectory", "") {

	BooksDB::Instance().loadNetworkLinks(myLinks);
	std::sort(myLinks.begin(), myLinks.end(), Comparator());

	UpdateLinks("http://data.fbreader.org/catalogs/generic-1.4.xml");
}

void NetworkLinkCollection::UpdateLinks(std::string genericUrl) {
	myGenericUrl = genericUrl;
	for (std::vector<shared_ptr<NetworkLink> >::iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		if ((*it)->getPredefinedId() == std::string()) {
			myTempCustomLinks.push_back(*it);
			pthread_mutex_t* m = new pthread_mutex_t();
			pthread_mutex_init(m, 0);
			myLocks[(*it)->SiteName] = m;
			myExists[(*it)->SiteName] = true;
		}
	}
	pthread_create(&myUpdThread, NULL, updLinks, (void*) this);
}

NetworkLinkCollection::~NetworkLinkCollection() {
	pthread_join(myUpdThread, NULL);
}

static std::string normalize(const std::string &url) {
	static const std::string PREFIX0 = "http://feedbooks.com/";
	static const std::string PREFIX1 = "http://www.feedbooks.com/";
	static const std::string STANZA_PREFIX = "http://feedbooks.com/book/stanza/";

	std::string nURL = url;
	if (ZLStringUtil::stringStartsWith(nURL, PREFIX1)) {
		nURL = PREFIX0 + nURL.substr(PREFIX1.length());
	}
	if (ZLStringUtil::stringStartsWith(nURL, STANZA_PREFIX)) {
		nURL = PREFIX0 + "book/" + nURL.substr(STANZA_PREFIX.length()) + ".epub";
	}
	return nURL;
}

std::string NetworkLinkCollection::makeBookFileName(const BookReference &reference) {
	myErrorMessage.clear();
	return makeBookFileName(::normalize(reference.cleanURL()), reference.BookFormat, reference.ReferenceType, false);
}

std::string NetworkLinkCollection::makeBookFileName(const std::string &url, BookReference::Format format, BookReference::Type type, bool createDirectories) {
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];
	std::string path = url.substr(url.find("://") + 3);
	if (ZLStringUtil::stringStartsWith(path, "www.")) {
		path.erase(0, 4);
	}

	size_t index = path.find(':');
	while (index != std::string::npos) {
		path[index] = '_';
		index = path.find(':', index);
	}

	shared_ptr<ZLDir> dir;
	std::string fileName = DirectoryOption.value();

	index = 0;
	while (true) {
		dir = ZLFile(fileName).directory(createDirectories);
		if (dir.isNull()) {
			if (createDirectories) {
				myErrorMessage = ZLStringUtil::printf(errorResource["couldntCreateDirectoryMessage"].value(), fileName);
			}
			return "";
		}
		size_t index2 = path.find('/', index);
		if (index2 == std::string::npos) {
			break;
		}
		if (index2 + 1 < path.size() && path[index2 + 1] == '?') {
			path.erase(index2, 1);
			break;
		}
		fileName = dir->itemPath(path.substr(index, index2 - index));
		index = index2 + 1;
	}

	size_t index2 = path.find('?', index);
	std::string name;
	std::string params;
	if (index2 != std::string::npos) {
		name = path.substr(index, index2 - index);
		params = path.substr(index2 + 1);
	} else {
		name = path.substr(index);
	}

	std::string ext;
	switch (format) {
		case BookReference::EPUB:
			ext = ".epub";
			break;
		case BookReference::MOBIPOCKET:
			ext = ".mobi";
			break;
		case BookReference::FB2_ZIP:
			ext = ".fb2.zip";
			break;
		case BookReference::NONE:
			break;
	}

	if (ext.empty()) {
		size_t j = name.rfind('.');
		if (j != std::string::npos) {
			ext = name.substr(j);
			name.erase(j);
		}
	} else if (ZLStringUtil::stringEndsWith(name, ext)) {
		name.erase(name.size() - ext.size());
	}

	index2 = 0;
	while (params.size() > index2) {
		size_t j = params.find('&', index2);

		std::string param = params.substr(index2, j - index2);
		if (!ZLStringUtil::stringStartsWith(param, "username=")
			&& !ZLStringUtil::stringStartsWith(param, "password=")) {

			name.append("_").append(param);
			const size_t backIndex = name.size() - 1;
			if (name[backIndex] == '=') {
				name.erase(backIndex);
			}
		}

		if (j == std::string::npos) {
			break;
		}
		index2 = j + 1;
	}
	if (type == BookReference::DOWNLOAD_DEMO) {
		name.append(".trial");
	}
	name.append(ext);
	return dir->itemPath(name);
}

std::string NetworkLinkCollection::bookFileName(const BookReference &reference) const {
	return BooksDB::Instance().getNetFile(::normalize(reference.cleanURL()));
}

bool NetworkLinkCollection::downloadBook(const BookReference &reference, std::string &fileName, const ZLNetworkSSLCertificate &sslCertificate, shared_ptr<ZLExecutionData::Listener> listener) {
	std::string nURL = ::normalize(reference.URL);
	rewriteUrl(nURL);
	const std::string nNetworkBookId = ::normalize(reference.cleanURL());
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];
	myErrorMessage.clear();

	if (nURL.empty() || nNetworkBookId.empty()) {
		myErrorMessage = errorResource["unknownErrorMessage"].value();
		return false;
	}
	std::string storedFileName = BooksDB::Instance().getNetFile(nNetworkBookId);
	if (!storedFileName.empty() && ZLFile(storedFileName).exists()) {
		fileName = storedFileName;
		return true;
	}
	fileName = makeBookFileName(nNetworkBookId, reference.BookFormat, reference.ReferenceType, true);
	if (fileName.empty()) {
		if (myErrorMessage.empty()) {
			myErrorMessage = errorResource["unknownErrorMessage"].value();
		}
		return false;
	}
	if (ZLFile(fileName).exists()) {
		ZLFile(fileName).remove();
	}
	myErrorMessage = ZLNetworkManager::Instance().downloadFile(nURL, sslCertificate, fileName, listener);
	if (!myErrorMessage.empty()) {
		return false;
	}
	BooksDB::Instance().setNetFile(nNetworkBookId, fileName);
	return true;
}

shared_ptr<NetworkBookCollection> NetworkLinkCollection::simpleSearch(const std::string &pattern) {
	ZLExecutionData::Vector dataList;
	std::vector<shared_ptr<NetworkOperationData> > opDataVector;
	shared_ptr<NetworkBookCollection> result;

	myErrorMessage.clear();

	for (std::vector<shared_ptr<NetworkLink> >::const_iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		NetworkLink &link = **it;
		if (link.isEnabled()) {
			shared_ptr<NetworkOperationData> opData = new NetworkOperationData(link);
			opDataVector.push_back(opData);
			shared_ptr<ZLExecutionData> data = link.simpleSearchData(*opData, pattern);
			if (!data.isNull()) {
				dataList.push_back(data);
			}
		}
	}

	while (myErrorMessage.empty() && !dataList.empty()) {
		myErrorMessage = ZLNetworkManager::Instance().perform(dataList);

		for (std::vector<shared_ptr<NetworkOperationData> >::const_iterator jt = opDataVector.begin(); jt != opDataVector.end(); ++jt) {
			NetworkOperationData &opData = **jt;
			if (!opData.Items.empty() && result.isNull()) {
				result = new NetworkBookCollection();
			}
			for (NetworkItem::List::const_iterator kt = opData.Items.begin(); kt != opData.Items.end(); ++kt) {
				result->addBook(*kt);
			}
		}

		dataList.clear();

		for (std::vector<shared_ptr<NetworkOperationData> >::const_iterator jt = opDataVector.begin(); jt != opDataVector.end(); ++jt) {
			shared_ptr<ZLExecutionData> data = (*jt)->resume();
			if (!data.isNull()) {
				dataList.push_back(data);
			}
		}
	}

	return result;
}

shared_ptr<NetworkBookCollection> NetworkLinkCollection::advancedSearch(const std::string &titleAndSeries, const std::string &author, const std::string &tag, const std::string &annotation) {
	ZLExecutionData::Vector dataList;
	std::vector<shared_ptr<NetworkOperationData> > opDataVector;
	shared_ptr<NetworkBookCollection> result;

	myErrorMessage.clear();

	for (std::vector<shared_ptr<NetworkLink> >::const_iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		NetworkLink &link = **it;
		if (link.isEnabled()) {
			shared_ptr<NetworkOperationData> opData = new NetworkOperationData(link);
			opDataVector.push_back(opData);
			shared_ptr<ZLExecutionData> data = link.advancedSearchData(*opData, titleAndSeries, author, tag, annotation);
			if (!data.isNull()) {
				dataList.push_back(data);
			}
		}
	}

	while (myErrorMessage.empty() && !dataList.empty()) {
		myErrorMessage = ZLNetworkManager::Instance().perform(dataList);

		for (std::vector<shared_ptr<NetworkOperationData> >::const_iterator jt = opDataVector.begin(); jt != opDataVector.end(); ++jt) {
			NetworkOperationData &opData = **jt;
			if (!opData.Items.empty() && result.isNull()) {
				result = new NetworkBookCollection();
			}
			for (NetworkItem::List::const_iterator kt = opData.Items.begin(); kt != opData.Items.end(); ++kt) {
				result->addBook(*kt);
			}
		}

		dataList.clear();

		for (std::vector<shared_ptr<NetworkOperationData> >::const_iterator jt = opDataVector.begin(); jt != opDataVector.end(); ++jt) {
			shared_ptr<ZLExecutionData> data = (*jt)->resume();
			if (!data.isNull()) {
				dataList.push_back(data);
			}
		}
	}

	return result;
}

size_t NetworkLinkCollection::size() const {
	return myLinks.size();
}

NetworkLink &NetworkLinkCollection::link(size_t index) const {
	return *myLinks[index];
}

size_t NetworkLinkCollection::numberOfEnabledLinks() const {
	size_t count = 0;
	for (std::vector<shared_ptr<NetworkLink> >::const_iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		if ((*it)->isEnabled()) {
			++count;
		}
	}
	return count;
}

void NetworkLinkCollection::rewriteUrl(std::string &url, bool externalUrl) const {
	const std::string host =
		ZLUnicodeUtil::toLower(ZLNetworkUtil::hostFromUrl(url));
	for (std::vector<shared_ptr<NetworkLink> >::const_iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		if (host.find((*it)->SiteName) != std::string::npos) {
			(*it)->rewriteUrl(url, externalUrl);
		}
	}
}
