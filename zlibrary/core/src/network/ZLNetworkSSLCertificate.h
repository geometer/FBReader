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

#ifndef __ZLNETWORKSSLCERTIFICATE_H__
#define __ZLNETWORKSSLCERTIFICATE_H__

#include <string>

class ZLNetworkSSLCertificate {

public:
	static const ZLNetworkSSLCertificate NULL_CERTIFICATE;
	static const ZLNetworkSSLCertificate DONT_VERIFY_CERTIFICATE;

public:
	const std::string Path;
	const bool DoVerify;

public:
	ZLNetworkSSLCertificate(const std::string &path);

private:
	ZLNetworkSSLCertificate(bool doVerify);

private:
	ZLNetworkSSLCertificate(const ZLNetworkSSLCertificate&);
	const ZLNetworkSSLCertificate &operator = (const ZLNetworkSSLCertificate&);
};

#endif /* __ZLNETWORKSSLCERTIFICATE_H__ */
