/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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
#include <ZLNetworkManager.h>

#include <ZLBase64EncodedImage.h>
#include <ZLNetworkImage.h>

#include "NetworkCatalogUtil.h"

#include "../network/NetworkLinkCollection.h"

shared_ptr<const ZLImage> NetworkCatalogUtil::getImageByNetworkUrl(const std::string &url, const std::string &prefix) {
	if (!ZLStringUtil::stringStartsWith(url, prefix)) {
		return 0;
	}

	return new ZLNetworkImage(ZLMimeType::IMAGE_AUTO, url);
}

shared_ptr<const ZLImage> NetworkCatalogUtil::getImageByDataUrl(const std::string &url) {
	if (!ZLStringUtil::stringStartsWith(url, "data:")) {
		return 0;
	}

	size_t index = url.find(',');
	if (index == std::string::npos) {
		return 0;
	}

	ZLBase64EncodedImage *image = new ZLBase64EncodedImage(ZLMimeType::IMAGE_AUTO);
	image->addData(url, index + 1, std::string::npos);
	return image;
}

shared_ptr<const ZLImage> NetworkCatalogUtil::getImageByUrl(const std::string &url) {
	shared_ptr<const ZLImage> image;

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

class NetworkImageDownloadListener : public ZLNetworkRequest::Listener {
public:
	NetworkImageDownloadListener(ZLTreeNode *node) : myNode(node) {}
	void finished(const std::string &error) {
		if (error.empty()) {
			myNode->updated();
		}
	}
private:
	ZLTreeNode *myNode;
};

shared_ptr<const ZLImage> NetworkCatalogUtil::getAndDownloadImageByUrl(const std::string &url, const ZLTreeNode *node) {
	shared_ptr<const ZLImage> image = getImageByUrl(url);

	shared_ptr<ZLNetworkRequest> downloadRequest = image->synchronizationData();
	if (!downloadRequest.isNull()) {
		downloadRequest->setListener(new NetworkImageDownloadListener(const_cast<ZLTreeNode*>(node)));
		ZLNetworkManager::Instance().performAsync(downloadRequest);
	}
	return image;
}
