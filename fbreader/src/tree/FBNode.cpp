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

const ZLTypeId FBNode::TYPE_ID(ZLTreeTitledNode::TYPE_ID);

std::map<std::string,shared_ptr<ZLImage> > FBNode::ourDefaultCovers;

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


