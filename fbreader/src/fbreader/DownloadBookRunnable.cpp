/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLDialogManager.h>

#include "DownloadBookRunnable.h"
#include "../network/NetworkLink.h"

DownloadBookRunnable::DownloadBookRunnable(const NetworkBookInfo &book, NetworkBookInfo::URLType format) {
	const std::map<NetworkBookInfo::URLType,std::string>::const_iterator it = book.URLByType.find(format);
	if (it != book.URLByType.end()) {
		myURL = it->second;
		myFileName = book.fileName(format);
	}
}

DownloadBookRunnable::DownloadBookRunnable(const std::string &url) : myURL(url) {
}

void DownloadBookRunnable::run() {
	myDownloadedFileName = NetworkLinkCollection::instance().downloadBook(myURL, myErrorMessage, myFileName);
}

const std::string &DownloadBookRunnable::executeWithUI() {
	ZLDialogManager::instance().wait(ZLResourceKey("downloadBook"), *this);
	return myDownloadedFileName;
}

const std::string &DownloadBookRunnable::errorMessage() {
	return myErrorMessage;
}
