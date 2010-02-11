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

#include <ZLStringUtil.h>

#include <ZLBase64EncodedImage.h>
#include <ZLNetworkImage.h>

#include "NetworkCatalogUtil.h"

#include "../network/NetworkLinkCollection.h"

shared_ptr<ZLImage> NetworkCatalogUtil::getImageByNetworkUrl(const std::string &url, const std::string &prefix) {
	if (!ZLStringUtil::stringStartsWith(url, prefix)) {
		return 0;
	}

	return new ZLNetworkImage("image/auto", url);
}

shared_ptr<ZLImage> NetworkCatalogUtil::getImageByDataUrl(const std::string &url) {
	if (!ZLStringUtil::stringStartsWith(url, "data:")) {
		return 0;
	}

	size_t index = url.find(',');
	if (index == std::string::npos) {
		return 0;
	}

	ZLBase64EncodedImage *image = new ZLBase64EncodedImage("image/auto");
	image->addData(url, index + 1, std::string::npos);
	return image;
}

shared_ptr<ZLImage> NetworkCatalogUtil::getImageByUrl(const std::string &url) {
	shared_ptr<ZLImage> image;

	image = getImageByNetworkUrl(url, "http://");
	if (!image.isNull()) {
		return image;
	}

	image = getImageByNetworkUrl(url, "https://");
	if (!image.isNull()) {
		return image;
	}

	image = getImageByDataUrl(url);
	if (!image.isNull()) {
		return image;
	}

	return 0;
}
