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

#ifndef __ZLCURLNETWORKNOACTIONDATA_H__
#define __ZLCURLNETWORKNOACTIONDATA_H__

#include "ZLCurlNetworkData.h"

class ZLCurlNetworkNoActionData : public ZLCurlNetworkData {

public:
	ZLCurlNetworkNoActionData(const std::string &url, const std::string &sslCertificate = std::string());

private:
	void onCancel();

	bool doBefore();
	void doAfter(bool success);
};

#endif /* __ZLCURLNETWORKNOACTIONDATA_H__ */
