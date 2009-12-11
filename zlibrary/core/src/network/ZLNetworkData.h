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

#ifndef __ZLNETWORKDATA_H__
#define __ZLNETWORKDATA_H__

#include <curl/curl.h>

#include <string>

#include <ZLExecutionData.h>

class ZLNetworkData : public ZLExecutionData {

public:
	static const std::string TYPE_ID;

protected:
	ZLNetworkData(const std::string &url, const std::string &sslCertificate = std::string());

public:
	virtual ~ZLNetworkData();
	const std::string &type() const;

	const std::string &url() const;
	const std::string &sslCertificate() const;
	CURL *handle();

	virtual bool doBefore() = 0;
	virtual void doAfter(bool success) = 0;

	const std::string &errorMessage() const;

protected:
	void setErrorMessage(const std::string &message);

private:
	const std::string myURL;
	const std::string mySSLCertificate;
	CURL *myHandle;
	std::string myErrorMessage;
};

#endif /* __ZLNETWORKDATA_H__ */
