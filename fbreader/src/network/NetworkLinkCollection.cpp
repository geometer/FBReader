/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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

#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLOutputStream.h>
#include <ZLStringUtil.h>
#include <ZLResource.h>

#include "NetworkLink.h"

#include "feedbooks/FeedBooksLink.h"
#include "fbreaderOrg/FBReaderOrgLink.h"

#include "../options/FBOptions.h"

NetworkLinkCollection *NetworkLinkCollection::ourInstance = 0;

NetworkLinkCollection &NetworkLinkCollection::instance() {
	if (ourInstance == 0) {
		ourInstance = new NetworkLinkCollection();
	}
	return *ourInstance;
}

NetworkLinkCollection::NetworkLinkCollection() :
	ConnectTimeoutOption(FBCategoryKey::NETWORK, "Options", "ConnectTimeout", 1, 1000, 10),
	TimeoutOption(FBCategoryKey::NETWORK, "Options", "Timeout", 1, 1000, 15),
	UseProxyOption(FBCategoryKey::NETWORK, "Options", "UseProxy", false),
	ProxyHostOption(FBCategoryKey::NETWORK, "Options", "ProxyHost", ""),
	ProxyPortOption(FBCategoryKey::NETWORK, "Options", "ProxyPort", "3128"),
	DirectoryOption(FBCategoryKey::NETWORK, "Options", "DownloadDirectory", "") {
	myLinks.push_back(new FeedBooksLink());
	myLinks.push_back(new FBReaderOrgLink());
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

std::string NetworkLinkCollection::bookFileName(const std::string &url) const {
	return ZLStringOption(FBCategoryKey::NETWORK, "Files", ::normalize(url), "").value();
}

std::string NetworkLinkCollection::downloadBook(const std::string &url, std::string &fileName) const {
	const std::string nURL = ::normalize(url);
	ZLStringOption fileNameOption(FBCategoryKey::NETWORK, "Files", nURL, "");
	std::string storedFileName = fileNameOption.value();
	if (!storedFileName.empty() && ZLFile(storedFileName).exists()) {
		fileName = storedFileName;
		return "";
	}

	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];
	shared_ptr<ZLDir> dir = ZLFile(DirectoryOption.value()).directory(true);
	if (dir.isNull()) {
		return ZLStringUtil::printf(errorResource["couldntCreateDirectoryMessage"].value(), DirectoryOption.value());
	}

	if (fileName.empty()) {
		size_t index = nURL.rfind('/');
		if (index >= nURL.length() - 1) {
			return errorResource["unknownErrorMessage"].value();
		}
		fileName = nURL.substr(index + 1);
	}
	fileName = dir->itemPath(fileName);
	if (ZLFile(fileName).exists()) {
		ZLFile(fileName).remove();
	}
	fileNameOption.setValue(fileName);
	return downloadFile(nURL, fileName);
}

std::string NetworkLinkCollection::perform(const std::vector<shared_ptr<CurlData> > &dataList) {
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];

	if (dataList.empty()) {
		return errorResource["emptyLibrariesList"].value();
	}

	const std::string proxy = ProxyHostOption.value() + ':' + ProxyPortOption.value();
	CURLM *handle = curl_multi_init();
	for (std::vector<shared_ptr<CurlData> >::const_iterator it = dataList.begin(); it != dataList.end(); ++it) {
		CURL *easyHandle = (*it)->handle();
		if (easyHandle != 0) {
			setStandardOptions(easyHandle, proxy);
			curl_multi_add_handle(handle, easyHandle);
		}
	}

	int counter;
	while ((curl_multi_perform(handle, &counter) == CURLM_CALL_MULTI_PERFORM) || (counter > 0)) {
	}
	curl_multi_cleanup(handle);

	return "";
}

std::string NetworkLinkCollection::simpleSearch(NetworkBookList &books, const std::string &pattern) {
	std::vector<shared_ptr<CurlData> > dataList;

	std::vector<NetworkBookList> bookLists(myLinks.size());
	int count = 0;

	for (LinkVector::const_iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		NetworkLink &link = **it;
		if (link.OnOption.value()) {
			dataList.push_back(link.simpleSearchData(bookLists[count++], pattern));
		}
	}

	std::string result = perform(dataList);

	for (std::vector<NetworkBookList>::const_iterator jt = bookLists.begin(); jt != bookLists.end(); ++jt) {
		books.insert(books.end(), jt->begin(), jt->end());
	}

	return result;
}

std::string NetworkLinkCollection::advancedSearch(NetworkBookList &books, const std::string &title, const std::string &author, const std::string &series, const std::string &tag, const std::string &annotation) {
	std::vector<shared_ptr<CurlData> > dataList;

	std::vector<NetworkBookList> bookLists(myLinks.size());
	int count = 0;

	for (LinkVector::const_iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		NetworkLink &link = **it;
		if (link.OnOption.value()) {
			dataList.push_back(link.advancedSearchData(bookLists[count++], title, author, series, tag, annotation));
		}
	}

	std::string result = perform(dataList);

	for (std::vector<NetworkBookList>::const_iterator jt = bookLists.begin(); jt != bookLists.end(); ++jt) {
		books.insert(books.end(), jt->begin(), jt->end());
	}

	return result;
}

static size_t writeToStream(void *ptr, size_t size, size_t nmemb, void *data) {
	((ZLOutputStream*)data)->write((const char*)ptr, size * nmemb);
	return size * nmemb;
}

void NetworkLinkCollection::setStandardOptions(CURL *handle, const std::string &proxy) const {
	if (UseProxyOption.value()) {
		curl_easy_setopt(handle, CURLOPT_PROXY, proxy.c_str());
	}
	curl_easy_setopt(handle, CURLOPT_TIMEOUT, TimeoutOption.value());
	curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, ConnectTimeoutOption.value());
}

std::string NetworkLinkCollection::downloadFile(const std::string &url, const std::string &fileName) const {
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];

	CURL *curl = curl_easy_init();
	if (!curl) {
		return errorResource["unknownErrorMessage"].value();
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	const std::string proxy = ProxyHostOption.value() + ':' + ProxyPortOption.value();
	setStandardOptions(curl, proxy);

	ZLFile fileToWrite(fileName);
	shared_ptr<ZLOutputStream> stream = fileToWrite.outputStream();
	if (stream.isNull() || !stream->open()) {
		return ZLStringUtil::printf(errorResource["couldntCreateFileMessage"].value(), fileName);
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToStream);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &*stream);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	stream->close();

	if (res == CURLE_OK) {
		return "";
	}

	fileToWrite.remove();

	switch (res) {
		default:
			return ZLStringUtil::printf(errorResource["errorDuringProcessingMessage"].value(), NetworkLink::hostFromUrl(url));
		case CURLE_COULDNT_RESOLVE_PROXY:
			return ZLStringUtil::printf(errorResource["couldntResolveProxyMessage"].value(), ProxyHostOption.value());
		case CURLE_COULDNT_RESOLVE_HOST:
			return ZLStringUtil::printf(errorResource["couldntResolveHostMessage"].value(), NetworkLink::hostFromUrl(url));
		case CURLE_COULDNT_CONNECT:
			return ZLStringUtil::printf(errorResource["couldntConnectMessage"].value(), NetworkLink::hostFromUrl(url));
		case CURLE_OPERATION_TIMEDOUT:
			return errorResource["operationTimedOutMessage"].value();
	}
}

size_t NetworkLinkCollection::size() const {
	return myLinks.size();
}

NetworkLink &NetworkLinkCollection::link(size_t index) const {
	return *myLinks[index];
}

bool NetworkLinkCollection::containsEnabledLinks() const {
	for (LinkVector::const_iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
		if ((*it)->OnOption.value()) {
			return true;
		}
	}
	return false;
}
