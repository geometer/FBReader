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

#ifndef __ZLCURLNETWORKMANAGER_H__
#define __ZLCURLNETWORKMANAGER_H__

#include <curl/curl.h>

#include "../../network/ZLNetworkManager.h"


class ZLNetworkRequest;
class ZLNetworkGetRequest;
class ZLNetworkPostRequest;


class ZLCurlNetworkManager : public ZLNetworkManager {

public:
	static void createInstance();

private:
	std::string perform(const ZLExecutionData::Vector &dataList) const;

	std::string doBeforeRequest(ZLNetworkRequest &request) const;
	std::string doBeforePostRequest(ZLNetworkPostRequest &request) const;

	void setStandardOptions(CURL *handle, const std::string &proxy) const;

	void setRequestOptions(CURL *handle, const ZLNetworkRequest &request) const;

	void clearRequestOptions(ZLNetworkRequest &request) const;
};

#endif /* __ZLCURLNETWORKMANAGER_H__ */
