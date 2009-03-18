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

#include <map>
#include <set>

#include <ZLStringUtil.h>
#include <ZLFile.h>
#include <ZLOutputStream.h>
#include <ZLResource.h>

#include "ZLNetworkManager.h"
#include "ZLNetworkUtil.h"

ZLNetworkManager *ZLNetworkManager::ourInstance = 0;

void ZLNetworkManager::deleteInstance() {
	if (ourInstance != 0) {
		delete ourInstance;
	}
}

ZLNetworkManager &ZLNetworkManager::instance() {
	if (ourInstance == 0) {
		ourInstance = new ZLNetworkManager();
	}
	return *ourInstance;
}

ZLNetworkManager::ZLNetworkManager() {
}

static const std::string OPTIONS = "Options";

ZLIntegerRangeOption &ZLNetworkManager::ConnectTimeoutOption() const {
	if (myConnectTimeoutOption.isNull()) {
		myConnectTimeoutOption = new ZLIntegerRangeOption(
			ZLCategoryKey::NETWORK, OPTIONS, "ConnectTimeout", 1, 1000, 10
		);
	}
	return *myConnectTimeoutOption;
}

ZLIntegerRangeOption &ZLNetworkManager::TimeoutOption() const {
	if (myTimeoutOption.isNull()) {
		myTimeoutOption = new ZLIntegerRangeOption(
			ZLCategoryKey::NETWORK, OPTIONS, "Timeout", 1, 1000, 15
		);
	}
	return *myTimeoutOption;
}

ZLBooleanOption &ZLNetworkManager::UseProxyOption() const {
	if (myUseProxyOption.isNull()) {
		myUseProxyOption = new ZLBooleanOption(
			ZLCategoryKey::NETWORK, OPTIONS, "UseProxy", false
		);
	}
	return *myUseProxyOption;
}

ZLStringOption &ZLNetworkManager::ProxyHostOption() const {
	if (myProxyHostOption.isNull()) {
		myProxyHostOption = new ZLStringOption(
			ZLCategoryKey::NETWORK, OPTIONS, "ProxyHost", ""
		);
	}
	return *myProxyHostOption;
}

ZLStringOption &ZLNetworkManager::ProxyPortOption() const {
	if (myProxyPortOption.isNull()) {
		myProxyPortOption = new ZLStringOption(
			ZLCategoryKey::NETWORK, OPTIONS, "ProxyPort", "3128"
		);
	}
	return *myProxyPortOption;
}

ZLNetworkManager::~ZLNetworkManager() {
}

bool ZLNetworkManager::connect() const {
	return true;
}

void ZLNetworkManager::release() const {
}

bool ZLNetworkManager::providesProxyInfo() const {
	return false;
}

bool ZLNetworkManager::useProxy() const {
	return UseProxyOption().value();
}

std::string ZLNetworkManager::proxyHost() const {
	return ProxyHostOption().value();
}

std::string ZLNetworkManager::proxyPort() const {
	return ProxyPortOption().value();
}

void ZLNetworkManager::setStandardOptions(CURL *handle, const std::string &proxy) const {
	if (useProxy()) {
		curl_easy_setopt(handle, CURLOPT_PROXY, proxy.c_str());
	}
	curl_easy_setopt(handle, CURLOPT_TIMEOUT, TimeoutOption().value());
	curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, ConnectTimeoutOption().value());
}

static size_t writeToStream(void *ptr, size_t size, size_t nmemb, void *data) {
	((ZLOutputStream*)data)->write((const char*)ptr, size * nmemb);
	return size * nmemb;
}

std::string ZLNetworkManager::downloadFile(const std::string &url, const std::string &fileName) const {
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];

	CURL *curl = curl_easy_init();
	if (!curl) {
		return errorResource["unknownErrorMessage"].value();
	}
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	const std::string proxy = proxyHost() + ':' + proxyPort();
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
			return ZLStringUtil::printf(errorResource["somethingWrongMessage"].value(), url);
		case CURLE_COULDNT_RESOLVE_PROXY:
			return ZLStringUtil::printf(errorResource["couldntResolveProxyMessage"].value(), proxyHost());
		case CURLE_COULDNT_RESOLVE_HOST:
			return ZLStringUtil::printf(errorResource["couldntResolveHostMessage"].value(), ZLNetworkUtil::hostFromUrl(url));
		case CURLE_COULDNT_CONNECT:
			return ZLStringUtil::printf(errorResource["couldntConnectMessage"].value(), ZLNetworkUtil::hostFromUrl(url));
		case CURLE_OPERATION_TIMEDOUT:
			return errorResource["operationTimedOutMessage"].value();
	}
}

std::string ZLNetworkManager::perform(const std::vector<shared_ptr<ZLNetworkData> > &dataList) const {
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];

	if (dataList.empty()) {
		return errorResource["emptyLibrariesList"].value();
	}

	const std::string proxy = proxyHost() + ':' + proxyPort();
	CURLM *handle = curl_multi_init();
	std::map<CURL*,shared_ptr<ZLNetworkData> > handleToData; 
	for (std::vector<shared_ptr<ZLNetworkData> >::const_iterator it = dataList.begin(); it != dataList.end(); ++it) {
		CURL *easyHandle = (*it)->handle();
		if (easyHandle != 0) {
			handleToData[easyHandle] = *it;
			setStandardOptions(easyHandle, proxy);
			curl_multi_add_handle(handle, easyHandle);
		}
	}

	int counter;
	CURLMcode res;
	do {
		res = curl_multi_perform(handle, &counter);
	} while ((res == CURLM_CALL_MULTI_PERFORM) || (counter > 0));

	CURLMsg *message;
	std::set<std::string> errors;
	do {
		int queueSize;
		message = curl_multi_info_read(handle, &queueSize);
		if ((message != 0) &&
		    (message->msg == CURLMSG_DONE) &&
		    (message->data.result != CURLE_OK)) {
			const std::string &url = handleToData[message->easy_handle]->url();
			switch (message->data.result) {
				default:
					errors.insert(ZLStringUtil::printf(errorResource["somethingWrongMessage"].value(), ZLNetworkUtil::hostFromUrl(url)));
					break;
				case CURLE_COULDNT_RESOLVE_PROXY:
					errors.insert(ZLStringUtil::printf(errorResource["couldntResolveProxyMessage"].value(), proxyHost()));
					break;
				case CURLE_COULDNT_RESOLVE_HOST:
					errors.insert(ZLStringUtil::printf(errorResource["couldntResolveHostMessage"].value(), ZLNetworkUtil::hostFromUrl(url)));
					break;
				case CURLE_COULDNT_CONNECT:
					errors.insert(ZLStringUtil::printf(errorResource["couldntConnectMessage"].value(), ZLNetworkUtil::hostFromUrl(url)));
					break;
				case CURLE_OPERATION_TIMEDOUT:
					errors.insert(errorResource["operationTimedOutMessage"].value());
					break;
			}
		}
	} while ((message != 0) && (errors.size() < 3));

	curl_multi_cleanup(handle);

	std::string result;
	for (std::set<std::string>::const_iterator et = errors.begin(); et != errors.end(); ++et) {
		if (!result.empty()) {
			result += '\n';
		}
		result += *et;
	}
	return result;
}
