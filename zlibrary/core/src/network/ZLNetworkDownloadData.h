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

#ifndef __ZLNETWORKDOWNLOADDATA_H__
#define __ZLNETWORKDOWNLOADDATA_H__

#include <shared_ptr.h>
#include <ZLNetworkData.h>
#include <ZLSlowProcess.h>

class ZLOutputStream;

class ZLNetworkDownloadData : public ZLNetworkData, public ZLSlowProcess {

private:
	static size_t handleHeader(void *ptr, size_t size, size_t nmemb, ZLNetworkDownloadData *self);
	static size_t writeToStream(void *ptr, size_t size, size_t nmemb, ZLNetworkDownloadData *self);

public:
	ZLNetworkDownloadData(const std::string &url, const std::string &fileName, shared_ptr<ZLOutputStream> stream = 0);
	ZLNetworkDownloadData(const std::string &url, const std::string &fileName, const std::string &sslCertificate, shared_ptr<ZLOutputStream> stream = 0);

private:
	void onCancel();

	bool doBefore();
	void doAfter(bool success);

private:
	const std::string myFileName;
	int myFileSize;
	int myDownloadedSize;
	shared_ptr<ZLOutputStream> myOutputStream;
};

#endif /* __ZLNETWORKDOWNLOADDATA_H__ */
