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

class ZLCurlNetworkManager : public ZLNetworkManager {

public:
	static void createInstance();

private:
	std::string perform(const ZLExecutionData::Vector &dataList) const;

	shared_ptr<ZLExecutionData> createNoActionData(const std::string &url, const std::string &sslCertificate) const;
	shared_ptr<ZLExecutionData> createNoActionData(const std::string &url) const;

	shared_ptr<ZLExecutionData> createReadToStringData(const std::string &url, std::string &dataString, const std::string &sslCertificate) const;
	shared_ptr<ZLExecutionData> createReadToStringData(const std::string &url, std::string &dataString) const;

	shared_ptr<ZLExecutionData> createDownloadData(const std::string &url, const std::string &fileName, const std::string &sslCertificate, shared_ptr<ZLOutputStream> stream = 0) const;
	shared_ptr<ZLExecutionData> createDownloadData(const std::string &url, const std::string &fileName, shared_ptr<ZLOutputStream> stream = 0) const;

	shared_ptr<ZLExecutionData> createXMLParserData(const std::string &url, const std::string &sslCertificate, shared_ptr<ZLXMLReader> reader) const;
	shared_ptr<ZLExecutionData> createXMLParserData(const std::string &url, shared_ptr<ZLXMLReader> reader) const;

	shared_ptr<ZLExecutionData> createPostFormData(const std::string &url, const std::string &sslCertificate, const std::vector<std::pair<std::string, std::string> > &formData) const;
	shared_ptr<ZLExecutionData> createPostFormData(const std::string &url, const std::vector<std::pair<std::string, std::string> > &formData) const;

private:
	void setStandardOptions(CURL *handle, const std::string &proxy) const;
};

#endif /* __ZLCURLNETWORKMANAGER_H__ */
