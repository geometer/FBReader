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
#include <cstring>
#include <algorithm>

#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLResource.h>
#include <ZLNetworkManager.h>
#include <ZLNetworkUtil.h>
#include <ZLibrary.h>
#include <ZLDialogManager.h>
#include <ZLDialog.h>
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
	BooksDB::Instance().deleteNetworkLink(link.SiteName);
	for (std::vector<shared_ptr<NetworkLink> >::iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		if (&(**it) == &link) {
			myLinks.erase(it);
			break;
		}
	}
	myExists.erase(link.SiteName);
	FBReader::Instance().invalidateNetworkView();
	FBReader::Instance().refreshWindow();
}

void NetworkLinkCollection::saveLink(NetworkLink& link, bool isAuto) {
	saveLinkWithoutRefreshing(link, isAuto);
	FBReader::Instance().refreshWindow();
}

class NetworkLinkCollection::AddNetworkCatalogScope : public ZLUserData {
public:
	shared_ptr<NetworkLink> link;
	shared_ptr<OPDSFeedReader> fr;
	shared_ptr<ZLXMLReader> prsr;
	shared_ptr<ZLExecutionData::Listener> listener;
};

void NetworkLinkCollection::addNetworkCatalogByUser(shared_ptr<ZLExecutionData::Listener> listener) {
	shared_ptr<ZLDialog> addDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("addNetworkCatalogDialog"));
	ZLStringOption URLOption(ZLCategoryKey::NETWORK, "url", "title", "");
	addDialog->addOption(ZLResourceKey("url"), URLOption);
	addDialog->addButton(ZLResourceKey("add"), true);
	addDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);

	if (addDialog->run()) {
		addDialog->acceptValues();
		addDialog.reset();
		std::string url = URLOption.value();
		AddNetworkCatalogScope *scope = new AddNetworkCatalogScope;
		scope->fr = new OPDSLink::FeedReader(scope->link, url);
		scope->prsr = new OPDSXMLParser(scope->fr);
		scope->listener = listener;
		shared_ptr<ZLExecutionData> request = ZLNetworkManager::Instance().createXMLParserRequest(url, scope->prsr);
		request->addUserData("scope", scope);
		request->setHandler(this, &NetworkLinkCollection::onNetworkCatalogReply);
		ZLNetworkManager::Instance().perform(request);
	} else {
		if (!listener.isNull())
			listener->finished(std::string());
	}
}

void NetworkLinkCollection::onNetworkCatalogReply(ZLUserDataHolder &data, const std::string &error) {
	// scope is not reference, because data is died somewhere at ZLDialog's run call a bit lower
	AddNetworkCatalogScope scope = static_cast<AddNetworkCatalogScope&>(*data.getUserData("scope"));
	const std::string message = ZLStringUtil::printf(ZLDialogManager::dialogMessage(ZLResourceKey("errorLinkBox")), error);
	if (scope.link == 0) {
		ZLDialogManager::Instance().informationBox(ZLResourceKey("errorLinkBox"), message);
		if (!scope.listener.isNull())
			scope.listener->finished(error);
		return;
	}

	shared_ptr<ZLDialog> checkDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("checkNetworkCatalogDialog"));
	ZLStringOption NameOption(ZLCategoryKey::NETWORK, "name", "title", "");
	NameOption.setValue(scope.link->getTitle());
	checkDialog->addOption(ZLResourceKey("name"), NameOption);
	ZLStringOption SubNameOption(ZLCategoryKey::NETWORK, "subname", "title", "");
	SubNameOption.setValue(scope.link->getSummary());
	checkDialog->addOption(ZLResourceKey("subname"), SubNameOption);
	ZLStringOption IconOption(ZLCategoryKey::NETWORK, "icon", "title", "");

	checkDialog->addButton(ZLResourceKey("add"), true);
	checkDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);
	if (checkDialog->run()) {
		checkDialog->acceptValues();
		checkDialog.reset();
		scope.link->setTitle(NameOption.value());
		scope.link->setSummary(SubNameOption.value());
		NetworkLinkCollection::Instance().saveLink(*scope.link);
	}
	if (!scope.listener.isNull())
		scope.listener->finished(std::string());
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
		FBReader::Instance().sendRefresh();
	}
}

NetworkLinkCollection::NetworkLinkCollection() :
	DirectoryOption(ZLCategoryKey::NETWORK, "Options", "DownloadDirectory", "") {

	BooksDB::Instance().loadNetworkLinks(myLinks);
	std::sort(myLinks.begin(), myLinks.end(), Comparator());

	updateLinks("http://data.fbreader.org/catalogs/generic-1.4.xml");
}

class NetworkLinkCollection::UpdateLinksScope : public ZLUserData {
public:
	std::vector<shared_ptr<NetworkLink> > links;
};

class NetworkLinkCollection::LoadLinkScope : public ZLUserData {
public:
	shared_ptr<NetworkLink> link;
};

void NetworkLinkCollection::updateLinks(std::string genericUrl) {
	myGenericUrl = genericUrl;
	for (std::vector<shared_ptr<NetworkLink> >::iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		if ((*it)->getPredefinedId() == std::string()) {
			myTempCustomLinks.push_back(*it);
			myExists.insert((*it)->SiteName);
		}
	}
	UpdateLinksScope *scope = new UpdateLinksScope;
	shared_ptr<OPDSFeedReader> fr = new OPDSLink::GenericFeedReader(scope->links);
	shared_ptr<ZLXMLReader> prsr = new OPDSXMLParser(fr);
	shared_ptr<ZLExecutionData> request = ZLNetworkManager::Instance().createXMLParserRequest(myGenericUrl, prsr);
	request->addUserData("scope", scope);
	request->setHandler(this, &NetworkLinkCollection::onLinksUpdated);
	ZLNetworkManager::Instance().perform(request);
}

void NetworkLinkCollection::onLinksUpdated(ZLUserDataHolder &data, const std::string &error) {
	(void) error;
	UpdateLinksScope &scope = static_cast<UpdateLinksScope&>(*data.getUserData("scope"));
	for (std::vector<shared_ptr<NetworkLink> >::iterator it = scope.links.begin(); it != scope.links.end(); ++it) {
		saveLinkWithoutRefreshing(**it, true);
	}
	for (std::vector<shared_ptr<NetworkLink> >::iterator it = myTempCustomLinks.begin(); it != myTempCustomLinks.end(); ++it) {
		LoadLinkScope *loadScope = new LoadLinkScope;
		std::string url = (*it)->url(NetworkLink::URL_MAIN);
		shared_ptr<OPDSFeedReader> fr = new OPDSLink::FeedReader(loadScope->link, url);
		shared_ptr<ZLXMLReader> prsr = new OPDSXMLParser(fr);
		shared_ptr<ZLExecutionData> request = ZLNetworkManager::Instance().createXMLParserRequest(url, prsr);
		request->addUserData("scope", loadScope);
		request->setHandler(this, &NetworkLinkCollection::onLinkLoaded);
		ZLNetworkManager::Instance().perform(request);
	}
}

void NetworkLinkCollection::onLinkLoaded(ZLUserDataHolder &data, const std::string &error) {
	(void) error;
	LoadLinkScope &scope = static_cast<LoadLinkScope&>(*data.getUserData("scope"));
	if (scope.link != 0) {
		if (myExists.find(scope.link->SiteName) != myExists.end()) {
			saveLinkWithoutRefreshing(*scope.link, true);
		}
	}
}

NetworkLinkCollection::~NetworkLinkCollection() {
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

std::string NetworkLinkCollection::bookFileName(const BookReference &reference) {
	myErrorMessage.clear();
	return bookFileName(::normalize(reference.cleanURL()), reference.BookFormat, reference.ReferenceType, false);
}

static bool parseUrl(const std::string &url, std::string &hostAndPath, std::string &query) {
	size_t hostBegin = url.find("://");
	if (hostBegin == std::string::npos)
		return false;
	hostBegin += 3;
	if (!url.compare(hostBegin, 4, "www."))
		hostBegin += 4;
	size_t pathEnd = url.find('?', hostBegin);
	hostAndPath = url.substr(hostBegin, pathEnd - hostBegin);
	if (pathEnd != std::string::npos)
		query = url.substr(pathEnd + 1);
	return true;
}

std::string NetworkLinkCollection::bookFileName(const std::string &url, BookReference::Format format, BookReference::Type type, bool createDirectories) {
	static const char escapeChars[] = "<>:\"|?*\\";

	std::string path;
	std::string query;
	if (!::parseUrl(url, path, query))
		return std::string();
	
	
	std::string fileName = DirectoryOption.value();
	if (!ZLStringUtil::stringEndsWith(fileName, ZLibrary::FileNameDelimiter))
		fileName += ZLibrary::FileNameDelimiter;
	if (type == BookReference::DOWNLOAD_DEMO) {
		fileName += "Demos";
		fileName += ZLibrary::FileNameDelimiter;
	}
	
	const size_t nameIndex = path.find_last_of('/');
	for (size_t i = 0; i < path.size(); ++i) {
		if (path[i] == '/') {
			if (ZLibrary::FileNameDelimiter.size() == 1)
				path[i] = ZLibrary::FileNameDelimiter[0];
			else
				path.replace(i, 1, ZLibrary::FileNameDelimiter);
		} else if (strchr(escapeChars, path[i])) {
			path[i] = '_';
		}
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
		// Remember for extensions like ".fb2.zip"
		size_t tmp = path.find('.', nameIndex);
		if (tmp == std::string::npos)
			return std::string();
		ext = path.substr(tmp);
		path.resize(tmp);
	} else if (ZLStringUtil::stringEndsWith(path, ext)) {
		path.resize(path.size() - ext.size());
	}
	fileName.append(path);
	
	if (!query.empty()) {
		size_t index = 0;
		for (;;) {
			size_t tmp = std::min(query.find('&', index), query.size());
			if (tmp < query.size())
				query[tmp] = '\0';
			char * const param = &query[0] + index;
			const int paramLength = tmp - index;
			// Both string's length is 9, so it's not a magic number
			if (strncmp(param, "username=", 9)
					&& strncmp(param, "password=", 9)
					&& param[paramLength - 1] != '=') {
				for (size_t i = 0; i < paramLength; ++i) {
					if (param[i] == '/' || strchr(escapeChars, param[i]))
						param[i] = '_';
				}
				fileName.append(1, '_');
				fileName.append(param, paramLength);
			}
			if (tmp >= query.size())
				break;
			index = tmp + 1;
		}
	}
	fileName += ext;
	if (createDirectories) {
		const size_t directoryIndex = fileName.find_last_of(ZLibrary::FileNameDelimiter);
		const std::string directoryPath = fileName.substr(0, directoryIndex);
		ZLFile(directoryPath).directory(true);
	}
	return fileName;
}

class BookDownloaderListener : public ZLExecutionData::Listener {
public:
	BookDownloaderListener(const std::string &bookId, const std::string &fileName, shared_ptr<ZLExecutionData::Listener> listener)
	    : myBookId(bookId), myFileName(fileName), myListener(listener) {
	}

	virtual ~BookDownloaderListener() {
	}
	
	void showPercent(int ready, int full) {
		myListener->showPercent(ready, full);
	}

	void finished(const std::string &error = std::string()) {
		myListener->finished(error);
	}

private:
	const std::string myBookId;
	const std::string myFileName;
	shared_ptr<ZLExecutionData::Listener> myListener;
};

bool NetworkLinkCollection::downloadBook(const BookReference &reference, std::string &fileName, const ZLNetworkSSLCertificate &sslCertificate, shared_ptr<ZLExecutionData::Listener> listener) {
	std::string nURL = ::normalize(reference.URL);
	rewriteUrl(nURL);
	const std::string nNetworkBookId = ::normalize(reference.cleanURL());
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];
	myErrorMessage.clear();

	if (nURL.empty() || nNetworkBookId.empty()) {
		myErrorMessage = errorResource["unknownErrorMessage"].value();
		listener->finished(myErrorMessage);
		return false;
	}
	fileName = bookFileName(nNetworkBookId, reference.BookFormat, reference.ReferenceType, true);
	if (ZLFile(fileName).exists()) {
		listener->finished();
		return true;
	}
	if (fileName.empty()) {
		if (myErrorMessage.empty()) {
			myErrorMessage = errorResource["unknownErrorMessage"].value();
		}
		listener->finished(myErrorMessage);
		return false;
	}
	if (ZLFile(fileName).exists()) {
		ZLFile(fileName).remove();
	}
	shared_ptr<ZLExecutionData::Listener> downloader = shared_ptr<ZLExecutionData::Listener>(
	            new BookDownloaderListener(nNetworkBookId, fileName, listener)
	            );
	myErrorMessage = ZLNetworkManager::Instance().downloadFile(nURL, sslCertificate, fileName, downloader);
	return myErrorMessage.empty();
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
