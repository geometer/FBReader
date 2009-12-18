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
#include <ZLibrary.h>

#include "ZLNetworkManager.h"
#include "ZLNetworkData.h"
#include "ZLNetworkUtil.h"
#include "ZLNetworkDownloadData.h"

ZLNetworkManager *ZLNetworkManager::ourInstance = 0;

void ZLNetworkManager::deleteInstance() {
	if (ourInstance != 0) {
		delete ourInstance;
	}
}

ZLNetworkManager &ZLNetworkManager::Instance() {
	return *ourInstance;
}

std::string ZLNetworkManager::CacheDirectory() {
	return
		ZLibrary::ApplicationWritableDirectory() +
		ZLibrary::FileNameDelimiter +
		"cache";
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

void ZLNetworkManager::setStandardOptions(void *curlHandle, const std::string &proxy) const {
	CURL *handle = (CURL*)curlHandle;
	static const char *AGENT_NAME = "FBReader";

	curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, true);
	curl_easy_setopt(handle, CURLOPT_USERAGENT, AGENT_NAME);
	if (useProxy()) {
		curl_easy_setopt(handle, CURLOPT_PROXY, proxy.c_str());
	}
	curl_easy_setopt(handle, CURLOPT_LOW_SPEED_LIMIT, 1L);
	curl_easy_setopt(handle, CURLOPT_LOW_SPEED_TIME, TimeoutOption().value());
	curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, ConnectTimeoutOption().value());
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 2L);
}

std::string ZLNetworkManager::downloadFile(const std::string &url, const std::string &fileName, shared_ptr<ZLSlowProcessListener> listener) const {
	return downloadFile(url, fileName, "", listener);
}

std::string ZLNetworkManager::downloadFile(const std::string &url, const std::string &fileName, const std::string &sslCertificate, shared_ptr<ZLSlowProcessListener> listener) const {
	ZLFile fileToWrite(fileName);
	shared_ptr<ZLOutputStream> stream = fileToWrite.outputStream(true);
	if (stream.isNull() || !stream->open()) {
		const ZLResource &errorResource =
			ZLResource::resource("dialog")["networkError"];
		return
			ZLStringUtil::printf(
				errorResource["couldntCreateFileMessage"].value(), fileName
			);
	}
	ZLExecutionData::Vector dataVector;
	ZLNetworkDownloadData *data = new ZLNetworkDownloadData(url, fileName, sslCertificate, stream);
	data->setListener(listener);
	dataVector.push_back(data);
	return perform(dataVector);
}

std::string ZLNetworkManager::perform(const ZLExecutionData::Vector &dataList) const {
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];

	if (dataList.empty()) {
		return errorResource["emptyLibrariesList"].value();
	}

	const std::string proxy = proxyHost() + ':' + proxyPort();
	CURLM *handle = curl_multi_init();
	std::map<CURL*,shared_ptr<ZLExecutionData> > handleToData; 
	for (ZLExecutionData::Vector::const_iterator it = dataList.begin(); it != dataList.end(); ++it) {
		if (it->isNull() || (*it)->type() != ZLNetworkData::TYPE_ID) {
			continue;
		}
		ZLNetworkData &nData = (ZLNetworkData&)**it;
		if (!nData.doBefore()) {
			continue;
		}
		CURL *easyHandle = nData.handle();
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
		if ((message != 0) && (message->msg == CURLMSG_DONE)) {
			ZLNetworkData &nData = (ZLNetworkData&)*handleToData[message->easy_handle];
			nData.doAfter(message->data.result == CURLE_OK);
			const std::string &url = nData.url();
			switch (message->data.result) {
				case CURLE_OK:
					break;
				case CURLE_WRITE_ERROR:
					if (!nData.errorMessage().empty()) {
						errors.insert(nData.errorMessage());
					} else {
						errors.insert(ZLStringUtil::printf(errorResource["somethingWrongMessage"].value(), ZLNetworkUtil::hostFromUrl(url)));
					}
					break;
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
				case CURLE_SSL_CONNECT_ERROR:
					errors.insert(ZLStringUtil::printf(errorResource["sslConnectErrorMessage"].value(), curl_easy_strerror(CURLE_SSL_CONNECT_ERROR)));
					break;
				case CURLE_PEER_FAILED_VERIFICATION:
					errors.insert(ZLStringUtil::printf(errorResource["peerFailedVerificationMessage"].value(), ZLNetworkUtil::hostFromUrl(url)));
					break;
				case CURLE_SSL_CACERT:
					errors.insert(ZLStringUtil::printf(errorResource["sslCertificateAuthorityMessage"].value(), ZLNetworkUtil::hostFromUrl(url)));
					break;
				case CURLE_SSL_CACERT_BADFILE:
					errors.insert(ZLStringUtil::printf(errorResource["sslBadCertificateFileMessage"].value(), nData.sslCertificate()));
					break;
				case CURLE_SSL_SHUTDOWN_FAILED:
					errors.insert(ZLStringUtil::printf(errorResource["sslShutdownFailedMessage"].value(), ZLNetworkUtil::hostFromUrl(url)));
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

shared_ptr<ZLExecutionData> ZLNetworkManager::createDownloadData(const std::string &url, const std::string &fileName) {
	return new ZLNetworkDownloadData(url, fileName);
}
