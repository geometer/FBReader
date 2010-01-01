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

#ifndef __ZLCURLNETWORKREADTOSTRINGDATA_H__
#define __ZLCURLNETWORKREADTOSTRINGDATA_H__

#include "ZLCurlNetworkData.h"

class ZLCurlNetworkReadToStringData : public ZLCurlNetworkData {

private:
	static size_t handleHeader(void *ptr, size_t size, size_t nmemb, ZLCurlNetworkReadToStringData *self);
	static size_t writeToString(void *ptr, size_t size, size_t nmemb, ZLCurlNetworkReadToStringData *self);

public:
	ZLCurlNetworkReadToStringData(const std::string &url, std::string &buffer, const std::string &sslCertificate = std::string());

private:
	void onCancel();

	bool doBefore();
	void doAfter(bool success);

private:
	std::string &myBuffer;
	size_t myDataSize;
};

#endif /* __ZLCURLNETWORKREADTOSTRINGDATA_H__ */
