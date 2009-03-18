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

#ifndef __NETWORKOPERATIONRUNNABLE_H__
#define __NETWORKOPERATIONRUNNABLE_H__

#include <string>

#include <ZLRunnable.h>

#include "../network/NetworkBookInfo.h"

class NetworkOperationRunnable : public ZLRunnable {

public:
	static void showErrorMessage(const std::string &message);

protected:
	NetworkOperationRunnable(const std::string &uiMessageKey);

public:
	void executeWithUI();
	bool hasErrors() const;
	void showErrorMessage() const;

protected:
	std::string myErrorMessage;

private:
	const std::string myUiMessageKey;
};

class DownloadBookRunnable : public NetworkOperationRunnable {

public:
	DownloadBookRunnable(const NetworkBookInfo &book, NetworkBookInfo::URLType format);
	DownloadBookRunnable(const std::string &url);
	void run();

	const std::string &fileName() const;

private:
	std::string myURL;
	std::string myFileName;
};

#endif /* __NETWORKOPERATIONRUNNABLE_H__ */
