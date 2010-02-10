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

#include <cstring>

#include <ZLibrary.h>
#include <ZLStringUtil.h>
#include <ZLNetworkManager.h>
#include <ZLDir.h>
#include <ZLFile.h>
#include <ZLFileImage.h>
#include <ZLResource.h>

#include <ZLOutputStream.h>

#include "ZLNetworkImage.h"

#include "../network/requests/ZLNetworkDownloadRequest.h"


class ImageDownloadRequest : public ZLNetworkDownloadRequest {

public:
	ImageDownloadRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, const std::string &fileName);

private:
	bool doAfter(bool success);
};

ImageDownloadRequest::ImageDownloadRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, const std::string &fileName) :
	ZLNetworkDownloadRequest(url, sslCertificate, fileName) {
}

bool ImageDownloadRequest::doAfter(bool success) {
	bool status = ZLNetworkDownloadRequest::doAfter(success);

	if (success && status) {
		shared_ptr<ZLImage> image = new ZLFileImage("image/auto", fileName(), 0);
		if (!image->good()) {
			image.reset();
			status = false;
			const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];
			setErrorMessage(ZLStringUtil::printf(errorResource["badImageFile"].value(), url()));
			ZLFile(fileName()).remove();
		}
	}
	return status;
}


ZLNetworkImage::ZLNetworkImage(const std::string &mimeType, const std::string &url) : ZLSingleImage(mimeType), myURL(url), myIsSynchronized(false) {
	static const std::string directoryPath = ZLNetworkManager::CacheDirectory();

	std::string prefix;
	if (ZLStringUtil::stringStartsWith(myURL, "http://")) {
		prefix = "http://";
	} else if (ZLStringUtil::stringStartsWith(myURL, "https://")) {
		prefix = "https://";
	} else if (ZLStringUtil::stringStartsWith(myURL, "ftp://")) {
		prefix = "ftp://";
	} else {
		myIsSynchronized = true;
		return;
	}
	myFileName = myURL.substr(prefix.length());
	myFileName = ZLFile::replaceIllegalCharacters(myFileName, '_');
	myFileName = directoryPath + ZLibrary::FileNameDelimiter + myFileName;

	static shared_ptr<ZLDir> dir = ZLFile(directoryPath).directory(true);
	if (dir.isNull()) {
		myIsSynchronized = true;
		return;
	}

	ZLFile imageFile(myFileName);
	if (imageFile.exists()) {
		myCachedImage = new ZLFileImage("image/auto", myFileName, 0);
		if (myCachedImage->good()) {
			myIsSynchronized = true;
		} else {
			myCachedImage.reset();
			imageFile.remove();
		}
	}
}

shared_ptr<ZLExecutionData> ZLNetworkImage::synchronizationData() const {
	if (myIsSynchronized) {
		return 0;
	}
	myIsSynchronized = true;
	return new ImageDownloadRequest(myURL, ZLNetworkSSLCertificate::NULL_CERTIFICATE, myFileName);
	//return ZLNetworkManager::Instance().createDownloadRequest(myURL, myFileName);
}

const shared_ptr<std::string> ZLNetworkImage::stringData() const {
	if (myCachedImage.isNull()) {
		ZLFile imageFile(myFileName);
		if (imageFile.exists()) {
			myCachedImage = new ZLFileImage("image/auto", myFileName, 0);
		}
	}
	return myCachedImage.isNull() ? 0 : myCachedImage->stringData();
}
