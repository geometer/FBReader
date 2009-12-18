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

#ifndef __ZLNETWORKMANAGER_H__
#define __ZLNETWORKMANAGER_H__

#include <string>

#include <shared_ptr.h>
#include <ZLOptions.h>

#include <ZLExecutionData.h>

class ZLNetworkData;
class ZLSlowProcessListener;
class ZLOutputStream;

class ZLNetworkManager {

public:
	static void deleteInstance();
	static ZLNetworkManager &Instance();

	static std::string CacheDirectory();

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
	std::string downloadFile(const std::string &url, const std::string &fileName, shared_ptr<ZLSlowProcessListener> listener = 0) const;
	std::string downloadFile(const std::string &url, const std::string &fileName, const std::string &sslCertificate, shared_ptr<ZLSlowProcessListener> listener = 0) const;
	// returns error message
	std::string perform(const ZLExecutionData::Vector &dataList) const;

public:
	virtual shared_ptr<ZLExecutionData> createDownloadData(const std::string &url, const std::string &fileName, const std::string &sslCertificate, shared_ptr<ZLOutputStream> stream = 0) const = 0;
	virtual shared_ptr<ZLExecutionData> createDownloadData(const std::string &url, const std::string &fileName, shared_ptr<ZLOutputStream> stream = 0) const = 0;

private:
	// void* instead of CURL* to avoid curl.h including
	void setStandardOptions(void *curlHandle, const std::string &proxy) const;

	mutable shared_ptr<ZLIntegerRangeOption> myConnectTimeoutOption;
	mutable shared_ptr<ZLIntegerRangeOption> myTimeoutOption;
	mutable shared_ptr<ZLBooleanOption> myUseProxyOption;
	mutable shared_ptr<ZLStringOption> myProxyHostOption;
	mutable shared_ptr<ZLStringOption> myProxyPortOption;
};

#endif /* __ZLNETWORKMANAGER_H__ */
