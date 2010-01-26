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

#include <map>
#include <set>

#include <ZLStringUtil.h>
#include <ZLNetworkUtil.h>
#include <ZLResource.h>
#include <ZLOutputStream.h>
#include <ZLXMLReader.h>

#include "ZLCurlNetworkManager.h"
#include "ZLCurlNetworkNoActionData.h"
#include "ZLCurlNetworkReadToStringData.h"
#include "ZLCurlNetworkDownloadData.h"
#include "ZLCurlNetworkXMLParserData.h"
#include "ZLCurlNetworkPostFormData.h"

void ZLCurlNetworkManager::createInstance() {
	ourInstance = new ZLCurlNetworkManager();
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createNoActionData(const std::string &url, const std::string &sslCertificate) const {
	return new ZLCurlNetworkNoActionData(url, sslCertificate);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createNoActionData(const std::string &url) const {
	return new ZLCurlNetworkNoActionData(url);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createReadToStringData(const std::string &url, std::string &dataString, const std::string &sslCertificate) const {
	return new ZLCurlNetworkReadToStringData(url, dataString, sslCertificate);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createReadToStringData(const std::string &url, std::string &dataString) const {
	return new ZLCurlNetworkReadToStringData(url, dataString);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createDownloadData(const std::string &url, const std::string &fileName, const std::string &sslCertificate, shared_ptr<ZLOutputStream> stream) const {
	return new ZLCurlNetworkDownloadData(url, fileName, sslCertificate, stream);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createDownloadData(const std::string &url, const std::string &fileName, shared_ptr<ZLOutputStream> stream) const {
	return new ZLCurlNetworkDownloadData(url, fileName, stream);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createXMLParserData(const std::string &url, const std::string &sslCertificate, shared_ptr<ZLXMLReader> reader) const {
	return new ZLCurlNetworkXMLParserData(url, sslCertificate, reader);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createXMLParserData(const std::string &url, shared_ptr<ZLXMLReader> reader) const {
	return new ZLCurlNetworkXMLParserData(url, reader);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createPostFormData(const std::string &url, const std::string &sslCertificate, const std::vector<std::pair<std::string, std::string> > &formData) const {
	return new ZLCurlNetworkPostFormData(url, sslCertificate, formData);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createPostFormData(const std::string &url, const std::vector<std::pair<std::string, std::string> > &formData) const {
	return new ZLCurlNetworkPostFormData(url, formData);
}

void ZLCurlNetworkManager::setStandardOptions(CURL *handle, const std::string &proxy) const {
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
	curl_easy_setopt(handle, CURLOPT_COOKIEJAR, CookiesPath().c_str());
}

std::string ZLCurlNetworkManager::perform(const ZLExecutionData::Vector &dataList) const {
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];

	if (dataList.empty()) {
		return errorResource["emptyLibrariesList"].value();
	}

	std::set<std::string> errors;

	const std::string proxy = proxyHost() + ':' + proxyPort();
	CURLM *handle = curl_multi_init();
	std::map<CURL*,shared_ptr<ZLExecutionData> > handleToData; 
	for (ZLExecutionData::Vector::const_iterator it = dataList.begin(); it != dataList.end(); ++it) {
		if (it->isNull() || (*it)->type() != ZLNetworkData::TYPE_ID) {
			continue;
		}
		ZLCurlNetworkData &nData = (ZLCurlNetworkData&)**it;
		if (!nData.doBefore()) {
			const std::string &err = nData.errorMessage();
			if (!err.empty()) {
				errors.insert(err);
			}
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
#if LIBCURL_VERSION_NUM > 0x071100
				case CURLE_PEER_FAILED_VERIFICATION:
#else
				case CURLE_SSL_PEER_CERTIFICATE:
#endif
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
