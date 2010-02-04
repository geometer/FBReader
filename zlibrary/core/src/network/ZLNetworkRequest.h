/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLNETWORKREQUEST_H__
#define __ZLNETWORKREQUEST_H__

#include <string>

#include <ZLExecutionData.h>
#include <ZLNetworkSSLCertificate.h>


class ZLNetworkRequest : public ZLExecutionData {

public:
	static const ZLTypeId TYPE_ID;

protected:
	ZLNetworkRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate);

private:
	const ZLTypeId &typeId() const;

public:
	virtual ~ZLNetworkRequest();

	const std::string &errorMessage() const;

	const std::string &url() const;
	const ZLNetworkSSLCertificate &sslCertificate() const;

protected:
	void setErrorMessage(const std::string &message);

public:
	virtual bool handleHeader(void *ptr, size_t size);
	virtual bool handleContent(void *ptr, size_t size) = 0;

	virtual bool doBefore() = 0;
	virtual void doAfter(bool success) = 0;

private:
	const std::string myURL;
	const ZLNetworkSSLCertificate mySSLCertificate;
	std::string myErrorMessage;

private: // disable copying
	ZLNetworkRequest(const ZLNetworkRequest &);
	const ZLNetworkRequest &operator = (const ZLNetworkRequest &);
};


class ZLNetworkGetRequest : public ZLNetworkRequest {

public:
	static const ZLTypeId TYPE_ID;

public:
	ZLNetworkGetRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate);

private:
	const ZLTypeId &typeId() const;
};


class ZLNetworkPostRequest : public ZLNetworkRequest {

public:
	static const ZLTypeId TYPE_ID;

public:
	ZLNetworkPostRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, 
		const std::vector<std::pair<std::string, std::string> > &postData);

	const std::vector<std::pair<std::string, std::string> > &postData() const;

private:
	const ZLTypeId &typeId() const;

private:
	std::vector<std::pair<std::string, std::string> > myData;
};

#endif /* __ZLNETWORKREQUEST_H__ */
