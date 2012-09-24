/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <algorithm>

#include <ZLNetworkManager.h>

#include "../../NetworkLink.h"
#include "../../NetworkComparators.h"
#include "../../NetworkErrors.h"
#include "../../NetworkItems.h"

#include "LitResUtil.h"
#include "LitResDataParser.h"

#include "LitResByGenresItem.h"

static const std::string EMPTY_STRING = std::string();

LitResByGenresItem::LitResByGenresItem(
	const std::vector<shared_ptr<LitResGenre> > &genreTree,
	const NetworkLink &link,
	const std::string &title,
	const std::string &summary,
	const std::map<URLType,std::string> &urlByType,
	VisibilityType visibility,
	int flags
) : NetworkCatalogItem(
	link,
	title,
	summary,
	urlByType,
	visibility,
	flags
), myGenreTree(genreTree) {
}

std::string LitResByGenresItem::loadChildren(NetworkItem::List &children) {
	for (size_t i = 0; i < myGenreTree.size(); ++i) {
		shared_ptr<LitResGenre> genre = myGenreTree.at(i);
		if (genre->Children.empty()) {
			children.push_back(new LitResBooksForGenreItem(*this, genre));
		} else {
			children.push_back(new LitResByGenresItem(genre->Children, Link, genre->Title, EMPTY_STRING, URLByType, Always, FLAG_NONE));
		}
	}
	return std::string();
}

LitResBooksForGenreItem::LitResBooksForGenreItem(const NetworkCatalogItem &parent, shared_ptr<LitResGenre> litresGenre)
	: NetworkCatalogItem(parent.Link, litresGenre->Title, EMPTY_STRING, parent.URLByType, Always), myLitresGenre(litresGenre) {
}

std::string LitResBooksForGenreItem::loadChildren(NetworkItem::List &children) {
	//TODO maybe add sid parameter if possible
	//(at LitRes API documentation it said that's adding sid _always_ is a good practice)
	shared_ptr<ZLExecutionData> data = ZLNetworkManager::Instance().createXMLParserRequest(
		LitResUtil::generateBooksByGenreUrl(myLitresGenre->Id),
		new LitResDataParser(Link, children)
	);
	std::string error = ZLNetworkManager::Instance().perform(data);
	std::sort(children.begin(), children.end(), NetworkBookItemComparator());
	return error;
}

