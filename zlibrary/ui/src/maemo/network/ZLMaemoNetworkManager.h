/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLMAEMONETWORKMANAGER_H__
#define __ZLMAEMONETWORKMANAGER_H__

#include <conic.h>

#include "../../../../core/src/unix/curl/ZLCurlNetworkManager.h"

class ZLMaemoNetworkManager : public ZLCurlNetworkManager {

public:
	static void createInstance();

private:
	ZLMaemoNetworkManager();
	~ZLMaemoNetworkManager();

public:
	void onStatusChanged(ConIcConnectionStatus status);

private:
	bool connect() const;
	void release() const;

	bool providesProxyInfo() const;

	bool useProxy() const;
	std::string proxyHost() const;
	std::string proxyPort() const;

private:
	mutable ConIcConnection *myConnection;
	mutable volatile enum {
		CONNECTED,
		FAILED,
		IN_PROGRESS
	} myConnectionStatus;
};

#endif /* __ZLMAEMONETWORKMANAGER_H__ */
