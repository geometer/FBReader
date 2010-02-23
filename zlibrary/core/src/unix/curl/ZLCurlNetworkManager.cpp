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

#include <curl/curl.h>

#include <ZLStringUtil.h>
#include <ZLNetworkUtil.h>
#include <ZLResource.h>
#include <ZLOutputStream.h>
#include <ZLXMLReader.h>
#include <ZLNetworkRequest.h>

#include "ZLCurlNetworkManager.h"



class PostData : public ZLUserData {

public:
	PostData();
	~PostData();

	bool addItem(const std::string &name, const std::string &content);

	const curl_httppost *postItem() const;

private:
	curl_httppost *myPostItem;
	curl_httppost *myLastItem;

private: // disable copying
	PostData(const PostData &);
	const PostData &operator = (const PostData &);
};

PostData::PostData() : myPostItem(0), myLastItem(0) {
}

PostData::~PostData() {
	if (myPostItem != 0) {
		curl_formfree(myPostItem);
	}
}

bool PostData::addItem(const std::string &name, const std::string &content) {
	// TODO: url-encode content???
	return curl_formadd(&myPostItem, &myLastItem,
		CURLFORM_COPYNAME, name.c_str(),
		CURLFORM_COPYCONTENTS, content.c_str(),
		CURLFORM_END) == 0;
}

inline const curl_httppost *PostData::postItem() const {
	return myPostItem;
}



static size_t handleHeader(void *ptr, size_t size, size_t nmemb, ZLNetworkRequest *request) {
	const size_t dataSize = size * nmemb;
	return (request->handleHeader(ptr, dataSize)) ? dataSize : 0;
}

static size_t handleContent(void *ptr, size_t size, size_t nmemb, ZLNetworkRequest *request) {
	const size_t dataSize = size * nmemb;
	return (request->handleContent(ptr, dataSize)) ? dataSize : 0;
}



void ZLCurlNetworkManager::createInstance() {
	ourInstance = new ZLCurlNetworkManager();
}


void ZLCurlNetworkManager::setStandardOptions(CURL *handle, const std::string &proxy) const {
	static const std::string AGENT_NAME = std::string("FBReader/") + VERSION;

	curl_easy_setopt(handle, CURLOPT_USERAGENT, AGENT_NAME.c_str());
	if (useProxy()) {
		curl_easy_setopt(handle, CURLOPT_PROXY, proxy.c_str());
	}
	curl_easy_setopt(handle, CURLOPT_LOW_SPEED_LIMIT, 1L);
	curl_easy_setopt(handle, CURLOPT_LOW_SPEED_TIME, TimeoutOption().value());
	curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, ConnectTimeoutOption().value());
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 2L);

	const std::string cookies = CookiesPath();
	curl_easy_setopt(handle, CURLOPT_COOKIEFILE, cookies.c_str());
	curl_easy_setopt(handle, CURLOPT_COOKIEJAR, cookies.c_str());
}


std::string ZLCurlNetworkManager::doBeforeRequest(ZLNetworkRequest &request) const {
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];

	if (!request.doBefore()) {
		const std::string &err = request.errorMessage();
		if (!err.empty()) {
			return err;
		}
		return ZLStringUtil::printf(errorResource["somethingWrongMessage"].value(), ZLNetworkUtil::hostFromUrl(request.url()));
	}

	if (request.isInstanceOf(ZLNetworkPostRequest::TYPE_ID)) {
		return doBeforePostRequest((ZLNetworkPostRequest &) request);
	}
	return "";
}

std::string ZLCurlNetworkManager::doBeforePostRequest(ZLNetworkPostRequest &request) const {
	shared_ptr<ZLUserData> postDataPtr = new PostData;
	PostData &postData = (PostData&)*postDataPtr;

	const std::vector<std::pair<std::string, std::string> > &data = request.postData();
	for (size_t i = 0; i < data.size(); ++i) {
		if (!postData.addItem(data[i].first, data[i].second)) {
			return "Invalid form data for " + ZLNetworkUtil::hostFromUrl(request.url()); // TODO: localize
		}
	}

	request.addUserData("postData", postDataPtr);
	return "";
}


void ZLCurlNetworkManager::setRequestOptions(CURL *handle, const ZLNetworkRequest &request) const {
	curl_easy_setopt(handle, CURLOPT_URL, request.url().c_str());
	if (!request.sslCertificate().Path.empty()) {
		curl_easy_setopt(handle, CURLOPT_CAINFO, request.sslCertificate().Path.c_str());
	} else if (!request.sslCertificate().DoVerify) {
		curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
	}

	curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, handleHeader);
	curl_easy_setopt(handle, CURLOPT_WRITEHEADER, &request);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, handleContent);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &request);

	switch (request.authenticationMethod()) {
	case ZLNetworkRequest::NO_AUTH:
		break;

	case ZLNetworkRequest::BASIC:
#if LIBCURL_VERSION_NUM >= 0x071301
		curl_easy_setopt(handle, CURLOPT_USERNAME, request.userName().c_str());
		curl_easy_setopt(handle, CURLOPT_PASSWORD, request.password().c_str());
#else
		curl_easy_setopt(
			handle, CURLOPT_USERPWD,
			(request.userName() + ':' + request.password()).c_str()
		);
#endif
		curl_easy_setopt(handle, CURLOPT_HTTPAUTH, (long) CURLAUTH_BASIC);
		break;
	}

	curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, request.isRedirectionSupported());

	if (request.isInstanceOf(ZLNetworkPostRequest::TYPE_ID)) {
		shared_ptr<ZLUserData> postDataPtr = request.getUserData("postData");
		PostData &postData = (PostData&)*postDataPtr;

		if (postData.postItem() != 0) {
			curl_easy_setopt(handle, CURLOPT_HTTPPOST, postData.postItem());
		}
	}
}


void ZLCurlNetworkManager::clearRequestOptions(ZLNetworkRequest &request) const {
	if (request.isInstanceOf(ZLNetworkPostRequest::TYPE_ID)) {
		request.removeUserData("postData");
	}
}



std::string ZLCurlNetworkManager::perform(const ZLExecutionData::Vector &dataList) const {
	const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];

	if (dataList.empty()) {
		return errorResource["emptyLibrariesList"].value();
	}

	std::set<std::string> errors;

	const std::string proxy = proxyHost() + ':' + proxyPort();
	CURLM *handle = curl_multi_init();

	std::map<CURL*,shared_ptr<ZLExecutionData> > handleToRequest;

	for (ZLExecutionData::Vector::const_iterator it = dataList.begin(); it != dataList.end(); ++it) {
		if (it->isNull() || !(*it)->isInstanceOf(ZLNetworkRequest::TYPE_ID)) {
			continue;
		}
		ZLNetworkRequest &request = (ZLNetworkRequest&)**it;
		const std::string err = doBeforeRequest(request);
		if (!err.empty()) {
			errors.insert(err);
			continue;
		}
		CURL *easyHandle = curl_easy_init();
		if (easyHandle != 0) {
			handleToRequest[easyHandle] = *it;
			setStandardOptions(easyHandle, proxy);
			setRequestOptions(easyHandle, request);
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
			ZLNetworkRequest &request = (ZLNetworkRequest&)*handleToRequest[message->easy_handle];
			const std::string &url = request.url();

			CURLcode result = message->data.result;
			bool doAfterResult = request.doAfter(result == CURLE_OK);
			if (result == CURLE_OK && !doAfterResult) {
				result = CURLE_WRITE_ERROR;
			}

			switch (result) {
				case CURLE_OK:
					break;
				case CURLE_WRITE_ERROR:
					if (!request.errorMessage().empty()) {
						errors.insert(request.errorMessage());
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
					errors.insert(ZLStringUtil::printf(errorResource["sslBadCertificateFileMessage"].value(), request.sslCertificate().Path));
					break;
				case CURLE_SSL_SHUTDOWN_FAILED:
					errors.insert(ZLStringUtil::printf(errorResource["sslShutdownFailedMessage"].value(), ZLNetworkUtil::hostFromUrl(url)));
					break;
			}
		}
	} while ((message != 0) && (errors.size() < 3));

	for (std::map<CURL*,shared_ptr<ZLExecutionData> >::const_iterator jt = handleToRequest.begin(); jt != handleToRequest.end(); ++jt) {
		CURL *easyHandle = jt->first;
		curl_multi_remove_handle(handle, easyHandle);
		curl_easy_cleanup(easyHandle);

		ZLNetworkRequest &request = (ZLNetworkRequest&)*jt->second;
		clearRequestOptions(request);
	}
	handleToRequest.clear();
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
