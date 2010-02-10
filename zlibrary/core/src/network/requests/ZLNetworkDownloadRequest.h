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

#ifndef __ZLNETWORKDOWNLOADREQUEST_H__
#define __ZLNETWORKDOWNLOADREQUEST_H__

#include "../ZLNetworkRequest.h"


class ZLOutputStream;

class ZLNetworkDownloadRequest : public ZLNetworkGetRequest {

public:
	ZLNetworkDownloadRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, const std::string &fileName);
	ZLNetworkDownloadRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, shared_ptr<ZLOutputStream> stream);

protected:
	bool handleHeader(void *ptr, size_t size);
	bool handleContent(void *ptr, size_t size);

	bool doBefore();
	bool doAfter(bool success);

	const std::string &fileName() const;

private:
	const std::string myFileName;
	int myFileSize;
	int myDownloadedSize;
	shared_ptr<ZLOutputStream> myOutputStream;
};

inline const std::string &ZLNetworkDownloadRequest::fileName() const { return myFileName; }

#endif /* __ZLNETWORKDOWNLOADREQUEST_H__ */
