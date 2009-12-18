/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLCURLNETWORKPOSTFORMDATA_H__
#define __ZLCURLNETWORKPOSTFORMDATA_H__

#include <string>

#include "ZLCurlNetworkData.h"

class ZLCurlNetworkPostFormData : public ZLCurlNetworkData {

public:
	ZLCurlNetworkPostFormData(const std::string &url, const std::vector<std::pair<std::string, std::string> > &formData);
	ZLCurlNetworkPostFormData(const std::string &url, const std::string &sslCertificate, const std::vector<std::pair<std::string, std::string> > &formData);
	~ZLCurlNetworkPostFormData();

	bool doBefore();
	void doAfter(bool success);

private:
	void init(const std::vector<std::pair<std::string, std::string> > &formData);

private:
	curl_httppost *myPostItem;
	curl_httppost *myLastItem;
};

#endif /* __ZLCURLNETWORKPOSTFORMDATA_H__ */
