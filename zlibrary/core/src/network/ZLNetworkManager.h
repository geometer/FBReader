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

#ifndef __ZLNETWORKMANAGER_H__
#define __ZLNETWORKMANAGER_H__

#include <string>

#include <shared_ptr.h>
#include <ZLOptions.h>

#include <ZLExecutionData.h>

class ZLNetworkData;
class ZLOutputStream;
class ZLXMLReader;

struct ZLNetworkSSLCertificate;

class ZLNetworkManager : public ZLExecutionData::Runner {

public:
	static void deleteInstance();
	static ZLNetworkManager &Instance();

	static std::string CacheDirectory();
	static std::string CookiesPath();

protected:
	static ZLNetworkManager *ourInstance;
	
public:
	ZLIntegerRangeOption &ConnectTimeoutOption() const;
	ZLIntegerRangeOption &TimeoutOption() const;
	ZLBooleanOption &UseProxyOption() const;
	ZLStringOption &ProxyHostOption() const;
	ZLStringOption &ProxyPortOption() const;

protected:
	ZLNetworkManager();
	virtual ~ZLNetworkManager();

public:
	virtual bool connect() const;
	virtual void release() const;

	virtual bool providesProxyInfo() const;

protected:
	virtual bool useProxy() const;
	virtual std::string proxyHost() const;
	virtual std::string proxyPort() const;

public:
	// returns error message
	std::string downloadFile(const std::string &url, const std::string &fileName, shared_ptr<ZLExecutionData::Listener> listener = 0) const;
	std::string downloadFile(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, const std::string &fileName, shared_ptr<ZLExecutionData::Listener> listener = 0) const;

public: 
	shared_ptr<ZLExecutionData> createDownloadRequest(const std::string &url, const std::string &fileName) const;
	shared_ptr<ZLExecutionData> createDownloadRequest(const std::string &url, shared_ptr<ZLOutputStream> stream) const;
	shared_ptr<ZLExecutionData> createDownloadRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, const std::string &fileName) const;
	shared_ptr<ZLExecutionData> createDownloadRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, shared_ptr<ZLOutputStream> stream) const;

	shared_ptr<ZLExecutionData> createNoActionRequest(const std::string &url) const;
	shared_ptr<ZLExecutionData> createNoActionRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate) const;

	shared_ptr<ZLExecutionData> createReadToStringRequest(const std::string &url, std::string &buffer) const;
	shared_ptr<ZLExecutionData> createReadToStringRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, std::string &buffer) const;

	shared_ptr<ZLExecutionData> createXMLParserRequest(const std::string &url, shared_ptr<ZLXMLReader> reader) const;
	shared_ptr<ZLExecutionData> createXMLParserRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, shared_ptr<ZLXMLReader> reader) const;

private:
	mutable shared_ptr<ZLIntegerRangeOption> myConnectTimeoutOption;
	mutable shared_ptr<ZLIntegerRangeOption> myTimeoutOption;
	mutable shared_ptr<ZLBooleanOption> myUseProxyOption;
	mutable shared_ptr<ZLStringOption> myProxyHostOption;
	mutable shared_ptr<ZLStringOption> myProxyPortOption;
};

#endif /* __ZLNETWORKMANAGER_H__ */
