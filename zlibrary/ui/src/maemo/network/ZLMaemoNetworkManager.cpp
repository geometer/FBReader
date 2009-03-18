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

#include <ZLStringUtil.h>

#include "ZLMaemoNetworkManager.h"

void ZLMaemoNetworkManager::createInstance() {
	ourInstance = new ZLMaemoNetworkManager();
}

ZLMaemoNetworkManager::ZLMaemoNetworkManager() {
	myConnection = con_ic_connection_new();
}

ZLMaemoNetworkManager::~ZLMaemoNetworkManager() {
	g_object_unref(myConnection);
}

void ZLMaemoNetworkManager::connect() const {
	con_ic_connection_connect(myConnection, CON_IC_CONNECT_FLAG_UNMANAGED);
}

void ZLMaemoNetworkManager::release() const {
}

bool ZLMaemoNetworkManager::providesProxyInfo() const {
	return true;
}

bool ZLMaemoNetworkManager::useProxy() const {
	return
		con_ic_connection_get_proxy_mode(myConnection) != CON_IC_PROXY_MODE_NONE;
}

std::string ZLMaemoNetworkManager::proxyHost() const {
	const char *host = con_ic_connection_get_proxy_host(myConnection, CON_IC_PROXY_PROTOCOL_HTTP);
	return (host != 0) ? host : std::string();
}

std::string ZLMaemoNetworkManager::proxyPort() const {
	int port = con_ic_connection_get_proxy_port(myConnection, CON_IC_PROXY_PROTOCOL_HTTP);
	std::string portString;
	ZLStringUtil::appendNumber(portString, port);
	return portString;
}
