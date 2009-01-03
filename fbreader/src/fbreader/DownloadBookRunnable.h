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

#ifndef __DOWNLOADBOOKRUNNABLE_H__
#define __DOWNLOADBOOKRUNNABLE_H__

#include <string>

#include <ZLRunnable.h>

#include "../network/NetworkBookInfo.h"

class DownloadBookRunnable : public ZLRunnable {

public:
	DownloadBookRunnable(const NetworkBookInfo &book, NetworkBookInfo::URLType format);
	DownloadBookRunnable(const std::string &url);
	void run();

	const std::string &executeWithUI();
	const std::string &errorMessage();

private:
	std::string myURL;
	std::string myFileName;
	std::string myErrorMessage;
	std::string myDownloadedFileName;
};

#endif /* __DOWNLOADBOOKRUNNABLE_H__ */
