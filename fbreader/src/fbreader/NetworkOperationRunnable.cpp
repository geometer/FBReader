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

#include <ZLDialogManager.h>

#include "NetworkOperationRunnable.h"
#include "../network/NetworkLink.h"

NetworkOperationRunnable::NetworkOperationRunnable(const std::string &uiMessageKey) : myUiMessageKey(uiMessageKey) {
}

void NetworkOperationRunnable::executeWithUI() {
	ZLDialogManager::instance().wait(ZLResourceKey(myUiMessageKey), *this);
}

bool NetworkOperationRunnable::hasErrors() const {
	return !myErrorMessage.empty();
}

void NetworkOperationRunnable::showErrorMessage(const std::string &message) {
	ZLDialogManager::instance().errorBox(
		ZLResourceKey("networkError"),
		message
	);
}

void NetworkOperationRunnable::showErrorMessage() const {
	if (!myErrorMessage.empty()) {
		showErrorMessage(myErrorMessage);
	}
}

DownloadBookRunnable::DownloadBookRunnable(const NetworkBookInfo &book, NetworkBookInfo::URLType format) : NetworkOperationRunnable("downloadBook") {
	const std::map<NetworkBookInfo::URLType,std::string>::const_iterator it = book.URLByType.find(format);
	if (it != book.URLByType.end()) {
		myURL = it->second;
		myFileName = book.fileName(format);
	}
}

DownloadBookRunnable::DownloadBookRunnable(const std::string &url) : NetworkOperationRunnable("downloadBook"), myURL(url) {
}

void DownloadBookRunnable::run() {
	myErrorMessage = NetworkLinkCollection::instance().downloadBook(myURL, myFileName);
}

const std::string &DownloadBookRunnable::fileName() const {
	return myFileName;
}
