/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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
#include <ZLOutputStream.h>

#include "NetworkLink.h"
#include "CurlData.h"

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
	DirectoryOption(FBCategoryKey::NETWORK, "Options", "DownloadDirectory", "") {
	myLinks.push_back(new FeedBooksLink());
	myLinks.push_back(new FBReaderOrgLink());
}

NetworkLinkCollection::~NetworkLinkCollection() {
}

bool NetworkLinkCollection::downloadBook(const NetworkBookInfo &book, NetworkBookInfo::URLType format, const std::string &fileName) {
	std::map<NetworkBookInfo::URLType,std::string>::const_iterator it = book.URLByType.find(format);
	if (it == book.URLByType.end()) {
		return false;
	}

	return downloadFile(it->second, fileName);
}

bool NetworkLinkCollection::perform(const std::vector<shared_ptr<CurlData> > &dataList) {
	if (dataList.empty()) {
		return false;
	}

	CURLM *handle = curl_multi_init();
	for (std::vector<shared_ptr<CurlData> >::const_iterator it = dataList.begin(); it != dataList.end(); ++it) {
		CURL *easyHandle = (*it)->handle();
		if (easyHandle != 0) {
			curl_multi_add_handle(handle, easyHandle);
		}
	}

	int counter;
	while ((curl_multi_perform(handle, &counter) == CURLM_CALL_MULTI_PERFORM) || (counter > 0)) {
	}
	curl_multi_cleanup(handle);

	return true;
}

bool NetworkLinkCollection::simpleSearch(NetworkBookList &books, const std::string &pattern) {
	std::vector<shared_ptr<CurlData> > dataList;

	const size_t size = myLinks.size();
	std::vector<NetworkBookList> bookLists(size);

	for (size_t i = 0; i < size; ++i) {
		if (myLinks[i]->OnOption.value()) {
			dataList.push_back(myLinks[i]->simpleSearchData(bookLists[i], pattern));
		}
	}

	bool code = perform(dataList);

	for (size_t i = 0; i < size; ++i) {
		books.insert(books.end(), bookLists[i].begin(), bookLists[i].end());
	}

	return code;
}

bool NetworkLinkCollection::advancedSearch(NetworkBookList &books, const std::string &title, const std::string &author, const std::string &series, const std::string &tag, const std::string &annotation) {
	std::vector<shared_ptr<CurlData> > dataList;

	const size_t size = myLinks.size();
	std::vector<NetworkBookList> bookLists(size);

	for (size_t i = 0; i < size; ++i) {
		if (myLinks[i]->OnOption.value()) {
			dataList.push_back(myLinks[i]->advancedSearchData(books, title, author, series, tag, annotation));
		}
	}

	bool code = perform(dataList);

	for (size_t i = 0; i < size; ++i) {
		books.insert(books.end(), bookLists[i].begin(), bookLists[i].end());
	}

	return code;
}

static size_t writeToStream(void *ptr, size_t size, size_t nmemb, void *data) {
	((ZLOutputStream*)data)->write((const char*)ptr, size * nmemb);
	return size * nmemb;
}

bool NetworkLinkCollection::downloadFile(const std::string &url, const std::string &fileName) {
	CURL *curl = curl_easy_init();
	if (!curl) {
		return false;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	ZLFile fileToWrite(fileName);
	shared_ptr<ZLOutputStream> stream = fileToWrite.outputStream();
	if (stream.isNull() || !stream->open()) {
		return false;
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToStream);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &*stream);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	stream->close();

	if (res != CURLE_OK) {
		fileToWrite.remove();
		return false;
	}

	return true;
}

size_t NetworkLinkCollection::size() const {
	return myLinks.size();
}

NetworkLink &NetworkLinkCollection::link(size_t index) const {
	return *myLinks[index];
}
