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

#include <ZLibrary.h>
#include <ZLFileImage.h>

#include "FBTree.h"

const ZLTypeId FBTree::TYPE_ID(ZLTreeTitledNode::TYPE_ID);

const ZLTypeId &FBTree::typeId() const {
	return TYPE_ID;
}

std::map<std::string,shared_ptr<const ZLImage> > FBTree::ourDefaultCovers;
std::map<std::string,std::string> FBTree::ourDefaultUrls;

// is there already any implementation of this stuff anywhere?
//TODO move it to some Utils class
static char hex_helper(int c) {
	static char tmp[] = "0123456789ABCDEF";
	return tmp[c];
}

static std::string percent_encoding(const std::string &str) {
	std::string result;
	for (size_t i = 0; i < str.size(); ++i) {
		const char c = str[i];
		if (str[i] == '\\' || str[i] == '/') {
			result += '/';
		} else if (isalpha(c) || isdigit(c) || c == '.' || c == '-' || c == '_' || c == '~') {
			result += str[i];
		} else {
			result += "%";
			result += hex_helper((c & 0xf0) >> 4);
			result += hex_helper(c & 0x0f);
		}
	}
	return result;
}

shared_ptr<const ZLImage> FBTree::defaultCoverImage(const std::string &id) {
	shared_ptr<const ZLImage> cover = ourDefaultCovers[id];
	if (cover.isNull()) {
		cover = new ZLFileImage(
			ZLFile(ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + id), 0
		);
		ourDefaultCovers[id] = cover;
	}
	return cover;
}

std::string FBTree::defaultImageUrl(const std::string &id) {
	std::string &url = ourDefaultUrls[id];
	if (url.empty()) {
		url = ZLibrary::ApplicationImageDirectory();
		url += "/";
		url += id;
		url = LOCALFILE_SCHEME + SCHEME_POSTFIX + percent_encoding(url);
	}
	return url;
}

FBTree::FBTree(ZLTreeNode *parent, size_t position) : ZLTreeTitledNode(parent, position)/*, myCoverImageIsStored(false)*/ { }

std::string FBTree::subtitle() const {
	std::string result;
	int count = 0;
	const ZLTreeNode::List &subNodes = children();
	ZLTreeNode::List::const_iterator it = subNodes.begin();
	for (; it != subNodes.end() && count < 3; ++it, ++count) {
		if (count > 0) {
			result += ", ";
		}
		result += ((const FBTree*)*it)->title();
	}
	if (it != subNodes.end()) {
		result += ", ...";
	}
	return result;
}

//shared_ptr<const ZLImage> FBTree::coverImage() const {
//	if (!myCoverImageIsStored) {
//		myCoverImageIsStored = true;
//		myStoredCoverImage = image();
//	}
//	return myStoredCoverImage;
//}

