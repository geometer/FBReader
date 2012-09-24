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

#ifndef __LITRESUTIL_H__
#define __LITRESUTIL_H__

#include <string>

#include "../NetworkItems.h"

class NetworkLink;

class LitResUtil {

public:
	static std::string url(const std::string &path);
	static std::string url(const NetworkLink &link, const std::string &path);
	static std::string url(const NetworkLink &link,  bool secure, const std::string &path);
	static std::string url(bool secure, const std::string &path);

	static std::string generateTrialUrl(std::string bookId);
	static std::string generatePurchaseUrl(const NetworkLink &link, const std::string &bookId);
	static std::string generateDownloadUrl(const std::string &bookId);
	static std::string generateBooksByGenreUrl(const std::string &genreId);
	static std::string generateBooksByAuthorUrl(const std::string &authorId);

public:
	static shared_ptr<NetworkItem> createLitResNode(std::string type, std::string rel,
													const NetworkLink &link, std::string title,
													std::string annotation, std::map<NetworkItem::URLType,std::string> urlMap,
													bool dependsOnAccount);

private:
	 LitResUtil();
};

#endif /* __LITRESUTIL_H__ */
