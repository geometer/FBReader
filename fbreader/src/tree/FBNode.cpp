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

#include <ZLResource.h>
#include <ZLibrary.h>
#include <ZLFileImage.h>

#include "FBNode.h"

#include "../fbreader/FBReader.h"
#include "../options/FBOptions.h"
#include "../options/FBTextStyle.h"

const std::string FBNode::COMMA_JOIN_SEPARATOR = ", "; //used for ZLStringUtil::join

const ZLTypeId FBNode::TYPE_ID(ZLTreeTitledNode::TYPE_ID);

std::map<std::string,shared_ptr<ZLImage> > FBNode::ourDefaultCovers;
std::map<std::string,std::string> FBNode::ourDefaultUrls;

const ZLTypeId &FBNode::typeId() const {
	return TYPE_ID;
}

const ZLResource &FBNode::resource() const {
	return ZLResource::resource("libraryView");
}

shared_ptr<ZLImage> FBNode::image() const {
	if (!myCoverImageIsStored) {
		myCoverImageIsStored = true;
		myStoredCoverImage = extractCoverImage();
	}
	return myStoredCoverImage;
}

shared_ptr<ZLImage> FBNode::extractCoverImage() const {
	return 0;
}

shared_ptr<ZLImage> FBNode::defaultCoverImage(const std::string &id) {
	shared_ptr<ZLImage> cover = ourDefaultCovers[id];
	if (cover.isNull()) {
		cover = new ZLFileImage(
			ZLFile(ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + id), 0
		);
		ourDefaultCovers[id] = cover;
	}
	return cover;
}

// is there already any implementation of this stuff anywhere?
static char hex_helper(char c) {
	static char tmp[] = "0123456789ABCDEF";
	return tmp[c];
}

static std::string percent_encoding(const std::string &str) {
	std::string result;
	for (int i = 0; i < str.size(); ++i) {
		const char c = str[i];
		if (str[i] == '\\') {
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

std::string FBNode::defaultImageUrl(const std::string &id) {
	std::string &url = ourDefaultUrls[id];
	if (url.empty()) {
		url = ZLibrary::ApplicationImageDirectory();
		url += "/";
		url += id;
		url = "file://" + percent_encoding(url);
	}
	return url;
}


